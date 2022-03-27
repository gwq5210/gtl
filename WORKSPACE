load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

git_repository(
  name = "googletest",
  remote = "https://github.com/google/googletest.git",
  tag = "release-1.11.0",
)

new_git_repository(
  name = "spdlog",
  remote = "https://github.com/gabime/spdlog.git",
  tag = "v1.9.2",
  build_file = "@//:spdlog.BUILD",
)

new_git_repository(
  name = "fmtlib",
  remote = "https://github.com/fmtlib/fmt.git",
  tag = "8.1.1",
  build_file = "@//:fmtlib.BUILD",
)