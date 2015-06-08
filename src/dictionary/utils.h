/*
 * Copyright 2008 Google Inc.
 * Copyright 2015 Tomasz Kociumaka
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UTILS_H
#define UTILS_H
/* If this is being built for a unit test. */
#ifdef UNIT_TESTING


/* Redirect assert to mock_assert() so assertions can be caught by cmocka. */
#ifdef assert
#undef assert
#endif /* assert */
#define assert(expression) \
    mock_assert((int)(expression), #expression, __FILE__, __LINE__)
void mock_assert(const int result, const char* expression, const char *file,
                 const int line);

/* Redirect calloc, free, malloc & realloc to test_*() so cmocka can
 * check for memory leaks. */
#ifdef calloc
#undef calloc
#endif /* calloc */
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#ifdef free
#undef free
#endif /* free */
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)
#ifdef malloc
#undef malloc
#endif /* malloc */
#define malloc(ptr) _test_malloc(ptr, __FILE__, __LINE__)
#ifdef realloc
#undef realloc
#endif /* realloc */
#define realloc(ptr) _test_realloc(ptr, __FILE__, __LINE__)
void* _test_calloc(const size_t number_of_elements, const size_t size,
                   const char* file, const int line);
void _test_free(void* const ptr, const char* file, const int line);
void* _test_malloc(const size_t size, const char* file, const int line);
void* _test_realloc(void* const ptr, const size_t size, const char* file,
                    const int line);

/* All functions in this object need to be exposed to the test application,
 * so redefine static to nothing. */
#define static

#endif /* UNIT_TESTING */
#endif /* UTILS_H*/

