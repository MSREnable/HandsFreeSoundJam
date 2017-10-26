/*
COPYRIGHT 2017 - PROPERTY OF TOBII AB
-------------------------------------
2017 TOBII AB - KARLSROVAGEN 2D, DANDERYD 182 53, SWEDEN - All Rights Reserved.

NOTICE:  All information contained herein is, and remains, the property of Tobii AB and its suppliers, if any.
The intellectual and technical concepts contained herein are proprietary to Tobii AB and its suppliers and may be
covered by U.S.and Foreign Patents, patent applications, and are protected by trade secret or copyright law.
Dissemination of this information or reproduction of this material is strictly forbidden unless prior written
permission is obtained from Tobii AB.
*/

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Tobii.StreamEngine
{
    public static class Interop
    {
        public const string stream_engine_dll = "tobii_stream_engine";

        public static string tobii_error_message(tobii_error_t result_code)
        {
            var pStr = tobii_error_message_internal(result_code);
            return Marshal.PtrToStringAnsi(pStr);
        }

        [DllImport(stream_engine_dll, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_error_message")]
        private static extern IntPtr tobii_error_message_internal(tobii_error_t result_code);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_get_api_version")]
        public static extern tobii_error_t tobii_get_api_version(out tobii_version_t version);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void tobii_log_func_t(IntPtr log_context, tobii_log_level_t level, string text);

        public static tobii_error_t tobii_api_create(out IntPtr api, tobii_custom_log_t custom_log)
        {
            return tobii_api_create(out api, IntPtr.Zero, custom_log);
        }

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_api_create")]
        private static extern tobii_error_t tobii_api_create(out IntPtr api, IntPtr customAlloc, tobii_custom_log_t custom_log); // Custom alloc doesn't make sense in .NET

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_api_destroy")]
        public static extern tobii_error_t tobii_api_destroy(IntPtr api);

        public static tobii_error_t tobii_enumerate_local_device_urls(IntPtr api, out List<string> device_urls)
        {
            var urls = new List<string>();
            tobii_device_url_receiver_t handler = (url, data) => { urls.Add(url); };
            var result = tobii_enumerate_local_device_urls_internal(api, handler, IntPtr.Zero);

            device_urls = urls;

            return result;
        }

        public enum tobii_device_generations_t
        {
            G5 = 0x00000002,
            IS3 = 0x00000004,
            IS4 = 0x00000008,
        }

        public static tobii_error_t tobii_enumerate_local_device_urls_ex(IntPtr api, out List<string> device_urls, uint device_generations)
        {
            var urls = new List<string>();
            tobii_device_url_receiver_t handler = (url, data) => { urls.Add(url); };
            var result = tobii_enumerate_local_device_urls_ex_internal(api, handler, IntPtr.Zero, device_generations);

            device_urls = urls;

            return result;
        }

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_enumerate_local_device_urls")]
        private static extern tobii_error_t tobii_enumerate_local_device_urls_internal(IntPtr api, tobii_device_url_receiver_t receiverFunction, IntPtr userData);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_enumerate_local_device_urls_ex")]
        private static extern tobii_error_t tobii_enumerate_local_device_urls_ex_internal(IntPtr api, tobii_device_url_receiver_t receiverFunction, IntPtr userData, uint deviceGenerations);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_device_create")]
        public static extern tobii_error_t tobii_device_create(IntPtr api, string url, out IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_device_create_ex")]
        private static extern tobii_error_t tobii_device_create_ex_internal(IntPtr api, string url, tobii_license_key_t[] license_keys, int license_count, [MarshalAs(UnmanagedType.LPArray)] tobii_license_validation_result_t[] licenseResults, out IntPtr device);

        public static tobii_error_t tobii_device_create_ex(IntPtr api, string url, string[] license_keys, List<tobii_license_validation_result_t> license_results, out IntPtr device)
        {
            var keys = new List<tobii_license_key_t>();

            foreach (var key in license_keys)
            {
                keys.Add(new tobii_license_key_t { license_key = key, size_in_bytes = new IntPtr(key.Length * 2) });
            }

            var license_results_array = new tobii_license_validation_result_t[license_keys.Length];
            var tobii_error = tobii_device_create_ex_internal(api, url, keys.ToArray(), keys.Count, license_results_array, out device);

            if (license_results != null)
            {
                license_results.InsertRange(0, license_results_array);
            }

            return tobii_error;
        }

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_get_feature_group")]
        public static extern tobii_error_t tobii_get_feature_group(IntPtr device, out tobii_feature_group_t feature_group);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_device_destroy")]
        public static extern tobii_error_t tobii_device_destroy(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_wait_for_callbacks")]
        public static extern tobii_error_t tobii_wait_for_callbacks(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_process_callbacks")]
        public static extern tobii_error_t tobii_process_callbacks(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_clear_callback_buffers")]
        public static extern tobii_error_t tobii_clear_callback_buffers(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_reconnect")]
        public static extern tobii_error_t tobii_reconnect(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_system_clock")]
        public static extern tobii_error_t tobii_system_clock(IntPtr api, out long timestamp_us);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_get_device_info")]
        public static extern tobii_error_t tobii_get_device_info(IntPtr device, out tobii_device_info_t device_info);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
           EntryPoint = "tobii_license_key_store")]
        public static extern tobii_error_t tobii_license_key_store(IntPtr device, IntPtr data, IntPtr size);

        public static tobii_error_t tobii_license_key_store(IntPtr device, byte[] license_key)
        {
            var ptr = Marshal.AllocHGlobal(license_key.Length);
            tobii_error_t result;

            try
            {
                Marshal.Copy(license_key, 0, ptr, license_key.Length);
                result = tobii_license_key_store(device, ptr, new IntPtr(license_key.Length));
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }

            return result;
        }

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "tobii_license_key_retrieve")]
        public static extern tobii_error_t tobii_license_key_retrieve(IntPtr device, tobii_data_receiver_t receiver);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_get_state_bool")]
        public static extern tobii_error_t tobii_get_state_bool(IntPtr device, tobii_state_t state, out tobii_state_bool_t value);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_notifications_subscribe")]
        public static extern tobii_error_t tobii_notifications_subscribe(IntPtr device, tobii_notifications_callback_t callback);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_notifications_unsubscribe")]
        public static extern tobii_error_t tobii_notifications_unsubscribe(IntPtr device);


        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_capability_supported")]
        public static extern tobii_error_t tobii_capability_supported(IntPtr device, tobii_capability_t capability, out bool supported);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_stream_supported")]
        public static extern tobii_error_t tobii_stream_supported(IntPtr device, tobii_stream_t stream, out bool supported);

        #region Streams

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_gaze_point_subscribe")]
        public static extern tobii_error_t tobii_gaze_point_subscribe(IntPtr device, tobii_gaze_point_callback_t callback);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_gaze_point_unsubscribe")]
        public static extern tobii_error_t tobii_gaze_point_unsubscribe(IntPtr device);


        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_gaze_origin_subscribe")]
        public static extern tobii_error_t tobii_gaze_origin_subscribe(IntPtr device, tobii_gaze_origin_callback_t callback);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_gaze_origin_unsubscribe")]
        public static extern tobii_error_t tobii_gaze_origin_unsubscribe(IntPtr device);


        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_eye_position_normalized_subscribe")]
        public static extern tobii_error_t tobii_eye_position_normalized_subscribe(IntPtr device, tobii_eye_position_normalized_callback_t callback);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_eye_position_normalized_unsubscribe")]
        public static extern tobii_error_t tobii_eye_position_normalized_unsubscribe(IntPtr device);


        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_user_presence_subscribe")]
        public static extern tobii_error_t tobii_user_presence_subscribe(IntPtr device, tobii_user_presence_callback_t callback);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_user_presence_unsubscribe")]
        public static extern tobii_error_t tobii_user_presence_unsubscribe(IntPtr device);


        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_head_pose_subscribe")]
        public static extern tobii_error_t tobii_head_pose_subscribe(IntPtr device, tobii_head_pose_callback_t callback);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_head_pose_unsubscribe")]
        public static extern tobii_error_t tobii_head_pose_unsubscribe(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_get_track_box")]
        public static extern tobii_error_t tobii_get_track_box(IntPtr device, out tobii_track_box_t track_box);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_update_timesync")]
        public static extern tobii_error_t tobii_update_timesync(IntPtr device);

        #endregion


        #region Wearable

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_wearable_data_subscribe")]
        public static extern tobii_error_t tobii_wearable_data_subscribe(IntPtr device, tobii_wearable_data_callback_t callback);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_wearable_data_unsubscribe")]
        public static extern tobii_error_t tobii_wearable_data_unsubscribe(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_get_lens_configuration")]
        public static extern tobii_error_t tobii_get_lens_configuration(IntPtr device, out tobii_lens_configuration_t configuration);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_set_lens_configuration")]
        public static extern tobii_error_t tobii_set_lens_configuration(IntPtr device, ref tobii_lens_configuration_t configuration);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_lens_configuration_writable")]
        public static extern tobii_error_t tobii_lens_configuration_writable(IntPtr device, out tobii_lens_configuration_writable_t writable);

        #endregion

        #region Config
        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_start")]
        public static extern tobii_error_t tobii_calibration_start(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_stop")]
        public static extern tobii_error_t tobii_calibration_stop(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_clear")]
        public static extern tobii_error_t tobii_calibration_clear(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_collect_data_2d")]
        public static extern tobii_error_t tobii_calibration_collect_data_2d(IntPtr device, float x, float y);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_collect_data_3d")]
        public static extern tobii_error_t tobii_calibration_collect_data_3d(IntPtr device, float x, float y, float z);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_discard_data_2d")]
        public static extern tobii_error_t tobii_calibration_discard_data_2d(IntPtr device, float x, float y);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_discard_data_3d")]
        public static extern tobii_error_t tobii_calibration_discard_data_3d(IntPtr device, float x, float y, float z);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_compute_and_apply")]
        public static extern tobii_error_t tobii_calibration_compute_and_apply(IntPtr device);

        public static tobii_error_t tobii_calibration_retrieve(IntPtr device, out byte[] calibration)
        {
            byte[] buffer = null;

            var result = tobii_calibration_retrieve(device, (data, size) =>
            {
                buffer = new byte[size.ToInt32()];
                Marshal.Copy(data, buffer, 0, (int)size);
            });

            calibration = buffer;

            return result;
        }

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_retrieve")]
        public static extern tobii_error_t tobii_calibration_retrieve(IntPtr device, tobii_data_receiver_t callback);

        public static tobii_error_t tobii_calibration_parse(IntPtr api, byte[] calibration, out tobii_calibration_point_data_t[] point_data_items)
        {
            var p = Marshal.AllocHGlobal(calibration.Length);
            Marshal.Copy(calibration, 0, p, calibration.Length);
            var result = tobii_calibration_parse(api, p, new IntPtr(calibration.Length), out point_data_items);
            Marshal.FreeHGlobal(p);

            return result;
        }

        public static tobii_error_t tobii_calibration_parse(IntPtr api, IntPtr calibration, IntPtr calibration_size, out tobii_calibration_point_data_t[] point_data_items)
        {
            int count;
            point_data_items = new tobii_calibration_point_data_t[512];
            var pinned_array = GCHandle.Alloc(point_data_items, GCHandleType.Pinned);
            var pItems = pinned_array.AddrOfPinnedObject();
            var result = tobii_calibration_parse_internal(api, calibration, calibration_size, pItems, 512, out count);
            pinned_array.Free();

            Array.Resize(ref point_data_items, count);
            return result;
        }

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_parse")]
        private static extern tobii_error_t tobii_calibration_parse_internal(IntPtr api, IntPtr calibration, IntPtr calibration_size, IntPtr point_data_items, int capacity, out int count);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calibration_apply")]
        public static extern tobii_error_t tobii_calibration_apply(IntPtr device, IntPtr data, IntPtr size);

        public static tobii_error_t tobii_calibration_apply(IntPtr device, byte[] calibration)
        {
            var ptr = Marshal.AllocHGlobal(calibration.Length);
            tobii_error_t result;

            try
            {
                Marshal.Copy(calibration, 0, ptr, calibration.Length);
                result = tobii_calibration_apply(device, ptr, new IntPtr(calibration.Length));
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }

            return result;
        }

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_calculate_display_area_basic")]
        public static extern tobii_error_t tobii_calculate_display_area_basic(IntPtr api, float width_mm, float height_mm, float offset_x_mm, ref tobii_geometry_mounting_t geometry_mounting, out tobii_display_area_t display_area );

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_get_display_area")]
        public static extern tobii_error_t tobii_get_display_area(IntPtr device, out tobii_display_area_t display_area);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_set_display_area")]
        public static extern tobii_error_t tobii_set_display_area(IntPtr device, ref tobii_display_area_t display_area);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_get_geometry_mounting")]
        public static extern tobii_error_t tobii_get_geometry_mounting(IntPtr device, out tobii_geometry_mounting_t geometry);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "tobii_get_device_name")]
        private static extern tobii_error_t tobii_get_device_name(IntPtr device, StringBuilder device_name);

        public static tobii_error_t tobii_get_device_name(IntPtr device, out string device_name)
        {
            var dn = new StringBuilder(64);
            var result = tobii_get_device_name(device, dn);
            device_name = dn.ToString();
            return result;
        }

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "tobii_set_device_name")]
        public static extern tobii_error_t tobii_set_device_name(IntPtr device, string device_name);


        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_enumerate_output_frequencies")]
        public static extern tobii_error_t tobii_enumerate_output_frequencies(IntPtr device, tobii_output_frequency_receiver_t receiver);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_set_output_frequency")]
        public static extern tobii_error_t tobii_set_output_frequency(IntPtr device, float output_frequency);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_get_output_frequency")]
        public static extern tobii_error_t tobii_get_output_frequency(IntPtr device, out float output_frequency);

        #endregion

        #region Advanced

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_gaze_data_subscribe")]
        public static extern tobii_error_t tobii_gaze_data_subscribe(IntPtr device, tobii_gaze_data_callback_t callback);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_gaze_data_unsubscribe")]
        public static extern tobii_error_t tobii_gaze_data_unsubscribe(IntPtr device);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_enumerate_illumination_modes")]
        public static extern tobii_error_t tobii_enumerate_illumination_modes(IntPtr device, tobii_illumination_mode_receiver_t receiver);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_get_illumination_mode")]
        private static extern tobii_error_t tobii_get_illumination_mode(IntPtr device, StringBuilder illumination_mode);

        public static tobii_error_t tobii_get_illumination_mode(IntPtr device, out string illumination_mode)
        {
            var im = new StringBuilder(64);
            var result = tobii_get_illumination_mode(device, im);
            illumination_mode = im.ToString();
            return result;
        }

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_set_illumination_mode")]
        public static extern tobii_error_t tobii_set_illumination_mode(IntPtr device, string illumination_mode);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_timesync")]
        public static extern tobii_error_t tobii_timesync(IntPtr device, out tobii_timesync_data_t timesync);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_digital_syncport_subscribe")]
        public static extern tobii_error_t tobii_digital_syncport_subscribe(IntPtr device, tobii_digital_syncport_callback_t callback);

        [DllImport(stream_engine_dll, CallingConvention = CallingConvention.Cdecl, EntryPoint = "tobii_digital_syncport_unsubscribe")]
        public static extern tobii_error_t tobii_digital_syncport_unsubscribe(IntPtr device);

        #endregion
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_version_t
    {
        public int major;
        public int minor;
        public int revision;
        public int build;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct tobii_device_info_t
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)] public string serial_number;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string model;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string generation;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)] public string firmware_version;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class tobii_custom_log_t
    {
        public IntPtr log_context;
        public Interop.tobii_log_func_t log_func;
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_data_receiver_t(IntPtr data, IntPtr size);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_device_url_receiver_t(string url, IntPtr user_data);

    public enum tobii_feature_group_t
    {
        TOBII_FEATURE_GROUP_BLOCKED,
        TOBII_FEATURE_GROUP_CONSUMER,
        TOBII_FEATURE_GROUP_CONFIG,
        TOBII_FEATURE_GROUP_PROFESSIONAL,
        TOBII_FEATURE_GROUP_INTERNAL,
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_license_key_t
    {
        [MarshalAs(UnmanagedType.LPWStr)]
        public string license_key;

        public IntPtr size_in_bytes;
    }

    public enum tobii_license_validation_result_t
    {
        TOBII_LICENSE_VALIDATION_RESULT_OK,
        TOBII_LICENSE_VALIDATION_RESULT_TAMPERED,
        TOBII_LICENSE_VALIDATION_RESULT_INVALID_APPLICATION_SIGNATURE,
        TOBII_LICENSE_VALIDATION_RESULT_NONSIGNED_APPLICATION,
        TOBII_LICENSE_VALIDATION_RESULT_EXPIRED,
        TOBII_LICENSE_VALIDATION_RESULT_PREMATURE,
        TOBII_LICENSE_VALIDATION_RESULT_INVALID_PROCESS_NAME,
        TOBII_LICENSE_VALIDATION_RESULT_INVALID_SERIAL_NUMBER,
        TOBII_LICENSE_VALIDATION_RESULT_INVALID_MODEL,
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct TobiiVector2
    {
        public float x;
        public float y;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct TobiiVector3
    {
        public float x;
        public float y;
        public float z;
    }

    public enum tobii_validity_t
    {
        TOBII_VALIDITY_INVALID = 0,
        TOBII_VALIDITY_VALID = 1
    }

    public enum tobii_field_presence_t
    {
        TOBII_FIELD_NOT_PRESENT = 0,
        TOBII_FIELD_PRESENT = 1,
    }

    public enum tobii_error_t
    {
        TOBII_ERROR_NO_ERROR,
        TOBII_ERROR_INTERNAL,
        TOBII_ERROR_INSUFFICIENT_LICENSE,
        TOBII_ERROR_NOT_SUPPORTED,
        TOBII_ERROR_NOT_AVAILABLE,
        TOBII_ERROR_CONNECTION_FAILED,
        TOBII_ERROR_TIMED_OUT,
        TOBII_ERROR_ALLOCATION_FAILED,
        TOBII_ERROR_INVALID_PARAMETER,
        TOBII_ERROR_CALIBRATION_ALREADY_STARTED,
        TOBII_ERROR_CALIBRATION_NOT_STARTED,
        TOBII_ERROR_ALREADY_SUBSCRIBED,
        TOBII_ERROR_NOT_SUBSCRIBED,
        TOBII_ERROR_BUFFER_TOO_SMALL,
        TOBII_ERROR_OPERATION_FAILED,
        TOBII_ERROR_FIRMWARE_NO_RESPONSE,
    }

    public enum tobii_log_level_t
    {
        TOBII_LOG_LEVEL_ERROR,
        TOBII_LOG_LEVEL_WARN,
        TOBII_LOG_LEVEL_INFO,
        TOBII_LOG_LEVEL_DEBUG,
        TOBII_LOG_LEVEL_TRACE,
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_notifications_callback_t(ref tobii_notification_t notification);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_gaze_point_callback_t(ref tobii_gaze_point_t gaze_point);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_gaze_origin_callback_t(ref tobii_gaze_origin_t gaze_origin);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_eye_position_normalized_callback_t(ref tobii_eye_position_normalized_t eye_position);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_user_presence_callback_t(tobii_user_presence_status_t status, long timestamp_us);

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_gaze_point_t
    {
        public long timestamp_us;
        public tobii_validity_t validity;
        public TobiiVector2 position;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_track_box_t
    {
        public TobiiVector3 front_upper_right_xyz;
        public TobiiVector3 front_upper_left_xyz;
        public TobiiVector3 front_lower_left_xyz;
        public TobiiVector3 front_lower_right_xyz;
        public TobiiVector3 back_upper_right_xyz;
        public TobiiVector3 back_upper_left_xyz;
        public TobiiVector3 back_lower_left_xyz;
        public TobiiVector3 back_lower_right_xyz;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_gaze_origin_t
    {
        public long timestamp_us;
        public tobii_validity_t left_validity;
        public TobiiVector3 left;
        public tobii_validity_t right_validity;
        public TobiiVector3 right;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_eye_position_normalized_t
    {
        public long timestamp_us;
        public tobii_validity_t left_validity;
        public TobiiVector3 left;
        public tobii_validity_t right_validity;
        public TobiiVector3 right;
    }

    public enum tobii_user_presence_status_t
    {
        TOBII_USER_PRESENCE_STATUS_UNKNOWN,
        TOBII_USER_PRESENCE_STATUS_AWAY,
        TOBII_USER_PRESENCE_STATUS_PRESENT,
    }

    public enum tobii_notification_type_t
    {
        TOBII_NOTIFICATION_TYPE_CALIBRATION_STATE_CHANGED,
        TOBII_NOTIFICATION_TYPE_EXCLUSIVE_MODE_STATE_CHANGED,
        TOBII_NOTIFICATION_TYPE_TRACK_BOX_CHANGED,
        TOBII_NOTIFICATION_TYPE_DISPLAY_AREA_CHANGED,
        TOBII_NOTIFICATION_TYPE_FRAMERATE_CHANGED,
        TOBII_NOTIFICATION_TYPE_POWER_SAVE_STATE_CHANGED,
        TOBII_NOTIFICATION_TYPE_DEVICE_PAUSED_STATE_CHANGED,
    }

    public enum tobii_notification_value_type_t
    {
        TOBII_NOTIFICATION_VALUE_TYPE_NONE,
        TOBII_NOTIFICATION_VALUE_TYPE_FLOAT,
        TOBII_NOTIFICATION_VALUE_TYPE_STATE,
        TOBII_NOTIFICATION_VALUE_TYPE_DISPLAY_AREA,
    }

    [StructLayout(LayoutKind.Explicit)]
    public struct tobii_notification_value_t
    {
        [FieldOffset(0)]
        public float float_;
        [FieldOffset(0)]
        public tobii_state_bool_t state;
        [FieldOffset(0)]
        public tobii_display_area_t display_area;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_notification_t
    {
        public tobii_notification_type_t type;
        public tobii_notification_value_type_t value_type;
        public tobii_notification_value_t value;
    }


    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_wearable_data_callback_t(ref tobii_wearable_data_t data);

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_wearable_eye_t
    {
        public tobii_validity_t gaze_origin_validity;
        public TobiiVector3 gaze_origin_mm_xyz;

        public tobii_validity_t gaze_direction_validity;
        public TobiiVector3 gaze_direction_normalized_xyz;

        public tobii_validity_t pupil_diameter_validity;
        public float pupil_diameter_mm;

        public tobii_validity_t eye_openness_validity;
        public float eye_openness;

        public tobii_validity_t pupil_position_in_sensor_area_validity;
        public TobiiVector2 pupil_position_in_sensor_area_xy;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_wearable_data_t
    {
        public long timestamp_tracker_us;
        public long timestamp_system_us;
        public uint frame_counter;
        public uint led_mode;
        public tobii_wearable_eye_t left;
        public tobii_wearable_eye_t right;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_lens_configuration_t
    {
        public TobiiVector3 left_xyz;
        public TobiiVector3 right_xyz;
    }

    public enum tobii_lens_configuration_writable_t
    {
        TOBII_LENS_CONFIGURATION_NOT_WRITABLE,
        TOBII_LENS_CONFIGURATION_WRITABLE,
    }

    public enum tobii_capability_t
    {
		TOBII_CAPABILITY_DISPLAY_AREA_WRITABLE,
		TOBII_CAPABILITY_CALIBRATION_2D,
		TOBII_CAPABILITY_CALIBRATION_3D,
		TOBII_CAPABILITY_PERSISTENT_STORAGE,
    }

    public enum tobii_stream_t
    {
		TOBII_STREAM_GAZE_POINT,
		TOBII_STREAM_GAZE_ORIGIN,
		TOBII_STREAM_EYE_POSITION_NORMALIZED,
		TOBII_STREAM_USER_PRESENCE,
		TOBII_STREAM_HEAD_POSE,
		TOBII_STREAM_WEARABLE,
		TOBII_STREAM_GAZE_DATA,
		TOBII_STREAM_DIGITAL_SYNCPORT,
		TOBII_STREAM_DIAGNOSTICS_IMAGE,
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_output_frequency_receiver_t(float output_frequency);

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_display_area_t
    {
        public TobiiVector3 top_left_mm_xyz;
        public TobiiVector3 top_right_mm_xyz;
        public TobiiVector3 bottom_left_mm_xyz;
    }

    public enum tobii_calibration_point_status_t
    {
        TOBII_CALIBRATION_POINT_STATUS_FAILED_OR_INVALID,
        TOBII_CALIBRATION_POINT_STATUS_VALID_BUT_NOT_USED_IN_CALIBRATION,
        TOBII_CALIBRATION_POINT_STATUS_VALID_AND_USED_IN_CALIBRATION,
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_calibration_point_data_t
    {
        public TobiiVector2 point_xy;

        public tobii_calibration_point_status_t left_status;
        public TobiiVector2 left_mapping_xy;

        public tobii_calibration_point_status_t right_status;
        public TobiiVector2 right_mapping_xy;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_geometry_mounting_t
    {
        public int guides;
        public float width_mm;
        public float angle_deg;

        public TobiiVector3 external_offset_mm_xyz;
        public TobiiVector3 internal_offset_mm_xyz;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_gaze_data_eye_t
    {
        public tobii_validity_t gaze_origin_validity;
        public TobiiVector3 gaze_origin_from_eye_tracker_mm_xyz;
        public TobiiVector3 eye_position_in_track_box_normalized_xyz;

        public tobii_validity_t gaze_point_validity;
        public TobiiVector3 gaze_point_from_eye_tracker_mm_xyz;
        public TobiiVector2 gaze_point_on_display_normalized_xy;

        public tobii_validity_t eyeball_center_validity;
        public TobiiVector3 eyeball_center_from_eye_tracker_mm_xyz;

        public tobii_validity_t pupil_validity;
        public float pupil_diameter_mm;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_gaze_data_t
    {
        public long timestamp_tracker_us;
        public long timestamp_system_us;
        public tobii_gaze_data_eye_t left;
        public tobii_gaze_data_eye_t right;
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_gaze_data_callback_t(ref tobii_gaze_data_t gaze_data);

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_timesync_data_t
    {
        public long system_start_us;
        public long system_end_us;
        public long tracker_us;
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_illumination_mode_receiver_t(string illumination_mode);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_digital_syncport_callback_t(uint signal, long timestamp_tracker_us, long timestamp_system_us);

    public enum tobii_state_t
    {
        TOBII_STATE_POWER_SAVE_ACTIVE,
        TOBII_STATE_REMOTE_WAKE_ACTIVE,
        TOBII_STATE_DEVICE_PAUSED,
        TOBII_STATE_EXCLUSIVE_MODE
    }

    public enum tobii_state_bool_t
    {
        TOBII_STATE_BOOL_FALSE,
        TOBII_STATE_BOOL_TRUE,
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tobii_head_pose_callback_t(ref tobii_head_pose_t head_pose);

    [StructLayout(LayoutKind.Sequential)]
    public struct tobii_head_pose_t
    {
        public long timestamp_us;
        public tobii_validity_t position_validity;
        public TobiiVector3 position_xyz;

        public tobii_validity_t rotation_x_validity;
        public tobii_validity_t rotation_y_validity;
        public tobii_validity_t rotation_z_validity;
        public TobiiVector3 rotation_xyz;
    }
}
