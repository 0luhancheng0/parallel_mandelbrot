.PHONY: clean allclean
clean:
	rm -f .*DS_Store
	rm -f *.png
	rm -rf ./plots
	rm -rf __pycache__/
	rm -f parallel-*.out
	rm -f serial-*.out
allclean: clean
	rm -rf parallel_output
	rm -rf serial_output
	rm -f *.pyc
