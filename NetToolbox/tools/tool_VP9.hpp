#ifndef __TOOL_VP9_HPP__
#define __TOOL_VP9_HPP__

#include <string>
#include <string_view>
#include <tchar.h>

#include <omp.h>
#include <libvpx/vpx/vpx_encoder.h>
#include <libvpx/video_writer.h>
#include <libvpx/tools_common.h>

#ifdef _UNICODE
using string_t = std::wstring;
using string_view_t = std::wstring_view;
#else
using string_t = std::string;
using string_view_t = std::string_view;
#endif



class tool_VP9 {
public:
	tool_VP9 (string_view_t path) {
		//vpx_codec_ctx_t codec;
		int frame_count = 0;
		const int fps = 30;
		const int bitrate = 200;
		int keyframe_interval = 0;
		int max_frames = 1000000000; // right?
		int frames_encoded = 0;
		const char *outfile_arg = NULL;

		const VpxInterface *encoder = get_vpx_encoder_by_name ("vp9");
		VpxVideoInfo info = { 0, 0, 0, { 0, 0 } };
		info.codec_fourcc = encoder->fourcc;
		info.frame_width = 1920;
		info.frame_height = 1080;
		info.time_base.numerator = 1;
		info.time_base.denominator = fps;

		if (info.frame_width <= 0 || info.frame_height <= 0 ||
			(info.frame_width % 2) != 0 || (info.frame_height % 2) != 0) {
			die ("Invalid frame size: %dx%d", info.frame_width, info.frame_height);
		}

		vpx_image_t raw;
		if (!vpx_img_alloc (&raw, VPX_IMG_FMT_YV12, info.frame_width, info.frame_height, 1)) {
			die ("Failed to allocate image.");
		}

		printf ("Using %s\n", vpx_codec_iface_name (encoder->codec_interface ()));

		vpx_codec_enc_cfg_t cfg;
		vpx_codec_err_t res = vpx_codec_enc_config_default (encoder->codec_interface (), &cfg, 0);
		if (res) die_codec (&codec, "Failed to get default codec config.");

		cfg.g_w = info.frame_width;
		cfg.g_h = info.frame_height;
		cfg.g_timebase.num = info.time_base.numerator;
		cfg.g_timebase.den = info.time_base.denominator;
		cfg.rc_target_bitrate = bitrate;
		cfg.g_error_resilient = VPX_ERROR_RESILIENT_DEFAULT; // right?

		VpxVideoWriter *writer = vpx_video_writer_open (outfile_arg, kContainerIVF, &info);
		if (!writer) die ("Failed to open %s for writing.", outfile_arg);

		if (vpx_codec_enc_init (&codec, encoder->codec_interface (), &cfg, 0))
			die_codec (&codec, "Failed to initialize encoder");

		// Encode frames.
		while (vpx_img_read (&raw, infile)) {
			int flags = 0;
			if (keyframe_interval > 0 && frame_count % keyframe_interval == 0)
				flags |= VPX_EFLAG_FORCE_KF;	
			encode_frame (&codec, &raw, frame_count++, flags, writer);
			frames_encoded++;
			if (max_frames > 0 && frames_encoded >= max_frames) break;
		}

		// Flush encoder.
		while (encode_frame (&codec, NULL, -1, 0, writer)) {
		}

		printf ("\n");
		fclose (infile);
		printf ("Processed %d frames.\n", frame_count);

		vpx_img_free (&raw);
		if (vpx_codec_destroy (&codec)) die_codec (&codec, "Failed to destroy codec.");

		vpx_video_writer_close (writer);
	}

private:
	static bool encode_frame (vpx_codec_ctx_t *codec, vpx_image_t *img, int frame_index, VpxVideoWriter *writer) {
		vpx_codec_iter_t iter = nullptr;
		const vpx_codec_err_t res = vpx_codec_encode (codec, img, frame_index, 1, 0, VPX_DL_GOOD_QUALITY);
		if (res != VPX_CODEC_OK) die_codec (codec, "Failed to encode frame");

		const vpx_codec_cx_pkt_t *pkt = vpx_codec_get_cx_data (codec, &iter);
		if (!pkt)
			return false;
		do {
			if (pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
				if (!vpx_video_writer_write_frame (writer, (uint8_t*) pkt->data.frame.buf, pkt->data.frame.sz, pkt->data.frame.pts)) {
					die_codec (codec, "Failed to write compressed frame");
				}
			}
		} while ((pkt = vpx_codec_get_cx_data (codec, &iter)) != nullptr);
		return true;
	}

	static int rgb24_to_yv12 (unsigned char* RGB, unsigned char* YV12, int width, int height) {
		unsigned char* _Y = YV12;
		unsigned char* _V = YV12 + width * height;
		unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
#pragma omp parallel for
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				int R = RGB[(j * width + i) * 3];
				int G = RGB[(j * width + i) * 3 + 1];
				int B = RGB[(j * width + i) * 3 + 2];
				float Y = 0.299f * (float) R + 0.587f * (float) G + 0.114f * (float) B;
				_Y[j * width + i] = (unsigned char) (Y < 0 ? 0 : (Y > 255 ? 255 : Y));
				if (j % 2 == 0 && i % 2 == 0) {
					float V = 0.615f * (float) R - 0.515f * (float) G - 0.100f * (float) B;
					_V[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char) (V < 0 ? 0 : (V > 255 ? 255 : V));
					float U = -0.147f * (float) R - 0.289f * (float) G + 0.436f * (float) B;
					_U[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char) (U < 0 ? 0 : (U > 255 ? 255 : U));
				}
			}
		}
		return 0;
	}
};

#endif //__TOOL_VP9_HPP__
