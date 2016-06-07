#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from subprocess import Popen, PIPE
import shutil


def main():
	for i in range(100):
		print ("---------- TEST RUN {:02d} ----------".format(i))
		process = Popen(["./spring-headless", "head.script"], stdout=PIPE)
		(output, err) = process.communicate()
		exit_code = process.wait()

		is_error = False
		with open("infolog.txt", 'r') as f:
			for line in f:
				if "error: 102" in line:
					is_error = True
					break
		new_name = "./infolog_{0:02d}_error.txt".format(i) if is_error else "./infolog_{0:02d}.txt".format(i)
		shutil.move("./infolog.txt", new_name)


if __name__ == "__main__":
    main()
