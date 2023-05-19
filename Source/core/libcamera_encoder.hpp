/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2020, Raspberry Pi (Trading) Ltd.
 *
 * libcamera_encoder.cpp - libcamera video encoding class.
 */

#include "core/libcamera_app.hpp"
#include "core/stream_info.hpp"
#include "core/video_options.hpp"
#include "curves.hpp"
#include "defines.hpp"
#include "encoder/encoder.hpp"


typedef std::function<void(void *, size_t, int64_t, bool)> EncodeOutputReadyCallback;
typedef std::function<void(libcamera::ControlList &)> MetadataReadyCallback;

class LibcameraEncoder : public LibcameraApp
{
public:

	using Stream = libcamera::Stream;
	using FrameBuffer = libcamera::FrameBuffer;

	LibcameraEncoder() : LibcameraApp(std::make_unique<VideoOptions>())
	{
	}

	void StartEncoder()
	{
		createEncoder();

		encoder_->SetInputDoneCallback(std::bind(&LibcameraEncoder::encodeBufferDone, this, std::placeholders::_1));
		encoder_->SetOutputReadyCallback(encode_output_ready_callback_);
	}
	// This is callback when the encoder gives you the encoded output data.
	void SetEncodeOutputReadyCallback(EncodeOutputReadyCallback callback) { encode_output_ready_callback_ = callback; }
	void SetMetadataReadyCallback(MetadataReadyCallback callback) { metadata_ready_callback_ = callback; }
#if defined(USE_USHORT)
	void EncodeBuffer(CompletedRequestPtr &completed_request, Stream *stream, const ushort transformArray[256 * 256])
#else
	void EncodeBuffer(CompletedRequestPtr &completed_request, Stream *stream, const uchar transformArray[256])
#endif
	{
		StreamInfo info = GetStreamInfo(stream);
		FrameBuffer *buffer = completed_request->buffers[stream];

		libcamera::Span span = Mmap(buffer)[0];
		void *mem = span.data();
		if (!buffer || !mem)
			throw std::runtime_error("no buffer to encode");

		uint ySize = info.width * info.height;
		uint uvSize = ySize >> 1;
		uint size = ySize + uvSize;

#if defined(COPY_TO_OPEN_CV)
		counter--;
		if (counter <= 0) {
			Buffer framebuf = BufferPool::Shared.rent(size);

			memcpy(framebuf.data(), mem, size);
			recognitionQueue.push(framebuf);

			counter = options_->frame_counter;
		}
#endif
		assert(encoder_);

		//transform(mem, transformArray, ySize);
		//toGrayScale((uint8_t *)mem, ySize);

		auto ts = completed_request->metadata.get(controls::SensorTimestamp);
		int64_t timestamp_ns = ts ? *ts : buffer->metadata().timestamp;
		{
			std::lock_guard<std::mutex> lock(encode_buffer_queue_mutex_);
			encode_buffer_queue_.push(completed_request); // creates a new reference
		}
		encoder_->EncodeBuffer(buffer->planes()[0].fd.get(), span.size(), mem, info, timestamp_ns / 1000);
	}
	VideoOptions *GetOptions() const { return static_cast<VideoOptions *>(options_.get()); }
	void StopEncoder() { encoder_.reset(); }
#if defined(USE_USHORT)
	ushort transformed[1036800];
#else
	uchar transformed[2073600];
#endif

protected:
	virtual void createEncoder()
	{
		StreamInfo info;
		VideoStream(&info);
		if (!info.width || !info.height || !info.stride)
			throw std::runtime_error("video steam is not configured");

		encoder_ = std::unique_ptr<Encoder>(Encoder::Create(GetOptions(), info));
	}
	std::unique_ptr<Encoder> encoder_;

private:
	void toGrayScale(uint8_t *mem, int ySize) {
		memset(mem + ySize, 128, ySize >> 1);
	}

	void encodeBufferDone(void *mem)
	{
		// If non-NULL, mem would indicate which buffer has been completed, but
		// currently we're just assuming everything is done in order. (We could
		// handle this by replacing the queue with a vector of <mem, completed_request>
		// pairs.)
		assert(mem == nullptr);
		{
			std::lock_guard<std::mutex> lock(encode_buffer_queue_mutex_);
			if (encode_buffer_queue_.empty())
				throw std::runtime_error("no buffer available to return");
			CompletedRequestPtr &completed_request = encode_buffer_queue_.front();
			if (metadata_ready_callback_ && !GetOptions()->metadata.empty())
				metadata_ready_callback_(completed_request->metadata);
			encode_buffer_queue_.pop(); // drop shared_ptr reference
		}
	}

	std::queue<CompletedRequestPtr> encode_buffer_queue_;
	std::mutex encode_buffer_queue_mutex_;
	EncodeOutputReadyCallback encode_output_ready_callback_;
	MetadataReadyCallback metadata_ready_callback_;

	int counter = options_->frame_counter;
	void transformByCurve(void *mem, uint size);
#if defined(USE_USHORT)
	void transform(void *mem, const ushort transformArray[256 * 256], uint size);
#else
	void transform(void *mem, const uchar transformArray[256], uint size);
#endif
};
