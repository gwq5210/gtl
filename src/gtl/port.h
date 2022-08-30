#pragma once

#define GTL_OFFSETOF(TYPE, MEMBER) (((size_t) & (((TYPE*)16)->MEMBER)) - 16)