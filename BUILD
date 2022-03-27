package(
  default_visibility = ["//visibility:public"],
)

cc_library(
  name = 'gtl',
  hdrs = glob(['src/**/*.h']),
  includes = ['src'],
  visibility = ["//visibility:public"],
)

cc_test(
  name = "gtl_test",
  srcs = glob([
      "src/**/*.cc",
  ]),
  visibility = [
      "//visibility:public",
  ],
  deps = [
      "@googletest//:gtest",
      "@googletest//:gtest_main",
      ":gtl",
      "@spdlog//:spdlog",
      "@fmtlib//:fmtlib",
  ],
)
