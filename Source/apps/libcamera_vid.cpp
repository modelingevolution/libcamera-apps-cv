/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2020, Raspberry Pi (Trading) Ltd.
 *
 * libcamera_vid.cpp - libcamera video record app.
 */

#include <chrono>
#include <poll.h>
#include <signal.h>
#include "core/defines.hpp"

#include "core/libcamera_encoder.hpp"
#include "output/output.hpp"
#include <cstdio>
#include <ctime>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <limits.h>

using namespace std::placeholders;

// Some keypress/signal handling.


static uchar GLOBAL_TRANSFORMARRAY[256];
static int signal_received;

static int get_key_or_signal(VideoOptions const *options, pollfd p[1])
{
	int key = 0;
	if (signal_received == SIGINT)
		return 'x';
	if (options->keypress)
	{
		poll(p, 1, 0);
		if (p[0].revents & POLLIN)
		{
			char *user_string = nullptr;
			size_t len;
			[[maybe_unused]] size_t r = getline(&user_string, &len, stdin);
			key = user_string[0];
		}
	}
	if (options->signal)
	{
		if (signal_received == SIGUSR1)
			key = '\n';
		else if (signal_received == SIGUSR2)
			key = 'x';
		signal_received = 0;
	}
	return key;
}

void signalHandler( int signum ) {
	LOG_ERROR("Interrupt signal (" << signum << ") received.\n");
	exit(signum);
}

void addSignalHandling() {
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGABRT, signalHandler);
	signal(SIGKILL, signalHandler);
	signal(SIGSEGV, signalHandler);
	signal(SIGFPE, signalHandler);
	signal(SIGUSR1, signalHandler);
	signal(SIGUSR2, signalHandler);
}

static int get_colourspace_flags(std::string const &codec)
{
	if (codec == "mjpeg" || codec == "yuv420")
		return LibcameraEncoder::FLAG_VIDEO_JPEG_COLOURSPACE;
	else
		return LibcameraEncoder::FLAG_VIDEO_NONE;
}




// The main even loop for the application.
void event_loop(LibcameraEncoder* app)
{
	addSignalHandling();


	VideoOptions const *options = app->GetOptions();
	std::unique_ptr<Output> output = std::unique_ptr<Output>(Output::Create(options));
	app->SetEncodeOutputReadyCallback(std::bind(&Output::OutputReady, output.get(), _1, _2, _3, _4));
	app->SetMetadataReadyCallback(std::bind(&Output::MetadataReady, output.get(), _1));

	app->OpenCamera();
	app->ConfigureVideo(get_colourspace_flags(options->codec));
	app->StartEncoder();
	app->StartCamera();
	auto start_time = std::chrono::high_resolution_clock::now();

	// Monitoring for keypresses and signals.
	pollfd p[1] = { { STDIN_FILENO, POLLIN, 0 } };

	for (unsigned int count = 0; ; count++)
	{
		LibcameraEncoder::Msg msg = app->Wait();
		if (msg.type == LibcameraApp::MsgType::Timeout)
		{
			LOG_ERROR("ERROR: Device timeout detected, attempting a restart!!!");
			app->StopCamera();
			app->StartCamera();
			continue;
		}
		if (msg.type == LibcameraEncoder::MsgType::Quit)
			return;
		else if (msg.type != LibcameraEncoder::MsgType::RequestComplete)
			throw std::runtime_error("unrecognised message!");
		int key = get_key_or_signal(options, p);
		if (key == '\n')
			output->Signal();

		LOG(2, "Viewfinder frame " << count);
		auto now = std::chrono::high_resolution_clock::now();
		bool timeout = !options->frames && options->timeout &&
					   (now - start_time > std::chrono::milliseconds(options->timeout));
		bool frameout = options->frames && count >= options->frames;
		if (timeout || frameout || key == 'x' || key == 'X')
		{
			if (timeout)
				LOG(1, "Halting: reached timeout of " << options->timeout << " milliseconds.");
			app->StopCamera(); // stop complains if encoder very slow to close
			app->StopEncoder();
			return;
		}
		cv::Mat frame;

		CompletedRequestPtr &completed_request = std::get<CompletedRequestPtr>(msg.payload);

		app->EncodeBuffer(completed_request, app->VideoStream(), GLOBAL_TRANSFORMARRAY);
	}
}


void opencv_loop(LibcameraApp *app)
{
	addSignalHandling();

	cv::Mat frame;
	cv::Mat frame2;
	cv::Mat inRangeFrame;

	cv::Mat *current = &frame;
	cv::Mat *prv = &frame2;

	long frameCounter = 0;
	int trianglesCounter=0;

	while (true)
	{
		if (!app->GetVideoFrame(*current))
		{
			continue;
		}

		if (frameCounter > 0)
		{
			std::vector<std::vector<cv::Point> > contours;
			std::vector<cv::Point> approx;

			
			cv::inRange(frame,
					cv::Scalar(0, 0, 0),
					cv::Scalar(100, 100, 100),
						inRangeFrame);

			cv::findContours(inRangeFrame,contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

			for (size_t i = 0; i < contours.size(); i++) {
				approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true)*0.02, true);
				if(approx.size()==3)
					trianglesCounter++;
			}

			std::cout<<"Counturs: "<< contours.size()<<" Triangles: "<< trianglesCounter <<std::endl;
		}

		cv::Mat *tmp = prv;
		prv = current;
		current = tmp;

		frameCounter++;
	}

	
}

void send_to_stream_loop(LibcameraApp *app) {
	addSignalHandling();

	std::chrono::milliseconds timestamp_ms;

	while (true) {
		LOG(2, "Size of queue for frames to save before save: " << app->saveQueue.size());
		Buffer buffer = app->saveQueue.pop();
		timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());



		BufferPool::Shared.returnBuffer(buffer);
	}
}

int main(int argc, char *argv[])
{
	try
	{
		addSignalHandling();

		LibcameraEncoder app;

		VideoOptions *options = app.GetOptions();
		if (options->Parse(argc, argv))
		{
			if (options->verbose >= 2)
				options->Print();

			std::thread mainThread(event_loop, &app);
			cpu_set_t mainCpuSet;
			CPU_ZERO(&mainCpuSet);
			CPU_SET(0, &mainCpuSet);
			pthread_setaffinity_np(mainThread.native_handle(), sizeof(cpu_set_t), &mainCpuSet);

			std::thread recognitionThread(opencv_loop, &app);
			cpu_set_t recognitionCpuSet;
			CPU_ZERO(&recognitionCpuSet);
			CPU_SET(1, &recognitionCpuSet);
			pthread_setaffinity_np(recognitionThread.native_handle(), sizeof(cpu_set_t), &recognitionCpuSet);

			std::thread sendingToStreamThread(send_to_stream_loop, &app);
			cpu_set_t sendingToStreamCpuSet;
			CPU_ZERO(&sendingToStreamCpuSet);
			CPU_SET(2, &sendingToStreamCpuSet);
			pthread_setaffinity_np(recognitionThread.native_handle(), sizeof(cpu_set_t), &sendingToStreamCpuSet);

			mainThread.join();
			recognitionThread.join();
			sendingToStreamThread.join();
		}
	}
	catch (std::exception const &e)
	{
		LOG_ERROR("ERROR: *** " << e.what() << " ***");
	}

	return 0;
}
