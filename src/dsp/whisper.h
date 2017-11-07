/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#ifndef WHISPER_H
#define WHISPER_H
#ifdef __cplusplus
extern "C" {
#endif
#ifndef EXPORT
#define EXPORT __declspec(dllexport)
#endif
EXPORT void whisper_start_audio();
EXPORT void whisper_stop_audio();
EXPORT void whisper_toggle();
EXPORT void whisper_next_chord_please();
EXPORT void whisper_nothing();
#ifdef __cplusplus
}
#endif
#endif
