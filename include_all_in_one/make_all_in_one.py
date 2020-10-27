#!/usr/bin/env python

import os
THIS_DIR = os.path.dirname(os.path.realpath(__file__))
FPLUS_INCLUDE_DIR = os.path.realpath(THIS_DIR + "/../include/fplus")


def is_fplus_include_line(code_line):
    return (   code_line.startswith("#include <fplus/")
            or code_line.startswith("#include \""))


def extract_fplus_include_file(code_line):
    return (
         code_line
        .replace("#include <fplus/", "")
        .replace("#include \"", "")
        .replace("\"", "")
        .replace(">", "")[:-1]
    )

ALLREADY_INCLUDED_FILES = []

def parse_one_file(filename):
    if filename in ALLREADY_INCLUDED_FILES:
        return ""
    ALLREADY_INCLUDED_FILES.append(filename)

    parsed_result = "\n//\n" + "// " + filename + "\n//\n\n"
    with open(FPLUS_INCLUDE_DIR + "/" + filename, "r") as f:
        lines = f.readlines()
        for code_line in lines:
            if is_fplus_include_line(code_line):
                new_file = extract_fplus_include_file(code_line)
                parsed_result = parsed_result + parse_one_file(new_file)
            else:
                if not "#pragma once" in code_line:
                    parsed_result = parsed_result + code_line
    return parsed_result

print("-- Generating include_all_in_one")
content = parse_one_file("fplus.hpp")
with open(THIS_DIR + "/include/fplus/fplus.hpp", "w") as f:
    f.write(content)

