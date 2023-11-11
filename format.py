import os


def main():
    # Get all files that end with .cpp or .h
    src_files = [f for f in os.listdir("src") if f.endswith(".cpp")]
    inc_files = [f for f in os.listdir("include") if f.endswith(".h")]
    test_files = [f for f in os.listdir("tests") if f.endswith((".cpp", ".h"))]
    tool_files = [f for f in os.listdir("tools") if f.endswith((".cpp", ".h"))]

    # Format all the files
    for f in src_files:
        os.system("clang-format -i src/" + f)
    for f in inc_files:
        os.system("clang-format -i include/" + f)
    for f in test_files:
        os.system("clang-format -i tests/" + f)
    for f in tool_files:
        os.system("clang-format -i tools/" + f)


if __name__ == "__main__":
    main()
