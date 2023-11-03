// Copyright 2023 Sneller, Inc.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#pragma once

//

#include <stdint.h>

//

#if !defined(IGUANA_STATIC)
  #if defined(_WIN32) && (defined(_MSC_VER) || defined(__MINGW32__))
    #ifdef IGUANA_EXPORTS
      #define IGUANA_API __declspec(dllexport)
    #else
      #define IGUANA_API __declspec(dllimport)
    #endif
  #elif defined(_WIN32) && defined(__GNUC__)
    #ifdef IGUANA_EXPORTS
      #define IGUANA_API __attribute__((__dllexport__))
    #else
      #define IGUANA_API __attribute__((__dllimport__))
    #endif
  #elif defined(__GNUC__)
    #define IGUANA_API __attribute__((__visibility__("default")))
  #endif
#endif

// Not defined for static builds, we just define it to nothing.
#if !defined(IGUANA_API)
  #define IGUANA_API
#endif

//

#define IGUANA_PROCESSOR_LITTLE_ENDIAN true
