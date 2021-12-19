#!/usr/bin/python3
import os, sys

def path_for_name(file_name):
	return os.path.join(sys.argv[4], file_name)

with (
	open(sys.argv[1], "r") as fp_i,
	open(sys.argv[2], "w") as fp_c,
	open(sys.argv[3], "w") as fp_h,
):
	print("#pragma bank 3\n", file = fp_c)
	print("// Auto-generated file. Please do not edit directly.\n", file = fp_c)
	print("#include <stdint.h>\n", file = fp_c)
	print("// Auto-generated file. Please do not edit directly.\n", file = fp_h)
	print("#ifndef __MESSAGE_CONSTS_H__\n#define __MESSAGE_CONSTS_H__\n", file = fp_h)

	for i in fp_i:
		i: str = i.strip()
		if "=" in i:
			kv = i.split("=", maxsplit=1)
			print("extern const char %s[];" % kv[0], file = fp_h)
			print("const char %s[] = {" % kv[0], file = fp_c)
			vb = kv[1].encode("CP437")
			s = "\t%d," % len(vb)
			for vbi in vb:
				s += " %d," % vbi
			print("%s\n};" % s, file = fp_c)

	print("#endif", file = fp_h);
