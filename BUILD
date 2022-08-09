package(
  default_visibility = ["//visibility:public"],
)

cc_library(
  name = 'gtl',
  hdrs = glob(['src/**/*.h']),
  srcs = glob(['src/gtl/net/*.cc']),
  includes = ['src'],
  visibility = ["//visibility:public"],
  deps = [
      "@spdlog//:spdlog",
      "@fmtlib//:fmtlib",
  ],
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
  ],
)

cc_test(
  name = "udp_client",
  srcs = glob([
      "examples/net/udp_client.cc",
  ]),
  visibility = [
      "//visibility:public",
  ],
  deps = [
      "@googletest//:gtest",
      "@googletest//:gtest_main",
      ":gtl",
  ],
)

cc_test(
  name = "udp_server",
  srcs = glob([
      "examples/net/udp_server.cc",
  ]),
  visibility = [
      "//visibility:public",
  ],
  deps = [
      "@googletest//:gtest",
      "@googletest//:gtest_main",
      ":gtl",
  ],
)

cc_test(
  name = "tcp_client",
  srcs = glob([
      "examples/net/tcp_client.cc",
  ]),
  visibility = [
      "//visibility:public",
  ],
  deps = [
      "@googletest//:gtest",
      "@googletest//:gtest_main",
      ":gtl",
  ],
)

cc_test(
  name = "tcp_server",
  srcs = glob([
      "examples/net/tcp_server.cc",
  ]),
  visibility = [
      "//visibility:public",
  ],
  deps = [
      "@googletest//:gtest",
      "@googletest//:gtest_main",
      ":gtl",
  ],
)