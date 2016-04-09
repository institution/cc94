#pragma once

#define THROW(exceptionClass, message) throw exceptionClass( (message), __FILE__, __LINE__)

