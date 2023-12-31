#include "pch.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

class ITape {
public:
	virtual int shift(int) = 0;
	virtual void to_begin() = 0;
	virtual void to_end() = 0;

	virtual void set_name(string) = 0;
	virtual string get_name() = 0;
	virtual void set_length(int) = 0;
	virtual void set_rw_delay(int) = 0;
	virtual void set_be_delay(int) = 0;
	virtual void set_shift_delay(int) = 0;

	virtual int read() = 0;
	virtual void write(int) = 0;

protected:
	int width = 12;

	string name;
	int length;
	int cur_cell = 0;

	int rw_delay;
	int be_delay;
	int shift_delay;
};

class Tape : public ITape {
public:
	int shift(int step) {
		Sleep(shift_delay*abs(step));
		cur_cell += step;
		return (cur_cell >= 0 && cur_cell < length ? cur_cell : -1);
	}
	void to_begin() {
		Sleep(be_delay);
		cur_cell = 0;
	}
	void to_end() {
		Sleep(be_delay);
		cur_cell = length - 1;
	}

	void set_name(string n) {
		name = n;
	}
	string get_name() {
		return name;
	}
	void set_length(int l) {
		length = l;
	}
	void set_rw_delay(int rw_d) {
		rw_delay = rw_d;
	}
	void set_be_delay(int be_d) {
		be_delay = be_d;
	}
	void set_shift_delay(int shift_d) {
		shift_delay = shift_d;
	}

	int read() {
		int value;
		ifstream in;
		in.open(name.c_str());
		if (in.is_open()) {
			in.seekg(cur_cell * width * sizeof(char), ios::beg);
			Sleep(rw_delay);
			in >> value;
		}
		in.close();
		return value;
	}

	void write(int value) {
		ofstream out;
		out.open(name.c_str(), ios::app);
		out.seekp(0, ios::beg);
		if (out.is_open()) {
			out.seekp(cur_cell * width * sizeof(char), ios::beg);
			Sleep(rw_delay);
			out << setw(width) << value;
		}
		out.close();
	}

	Tape(string n, int l, int rw_d, int be_d, int shift_d) {
		set_name(n);
		set_length(l);
		set_rw_delay(rw_d);
		set_be_delay(be_d);
		set_shift_delay(shift_d);
	}

	Tape() {}
};

void config(int &, int &, int &);

void copy_in_RAM(Tape &, int* (&), int);

void copy_from_RAM(int* (&), Tape &, int);

void create_tape(Tape &, string, int, int, int, int);

void sort(int* (&), int);

void merge(Tape* (&), int, Tape &, int* (&));

class DoSort {
public:
	void sort_tape(Tape tape_in, Tape tape_out, int N, int M) {
		int max_amount = M / 4 <= N ? M / 4 : N;
		int* RAM = new int[M / 4];

		int cur_tmp_amount = 0;
		int tmp_amount = N / max_amount + (N % max_amount == 0 ? 0 : 1);
		Tape* tmp_tapes = new Tape[tmp_amount];

		int rw_delay, be_delay, shift_delay;
		config(rw_delay, be_delay, shift_delay);

		for (int i = 0; i < N / max_amount; i++) {
			copy_in_RAM(tape_in, RAM, max_amount);
			sort(RAM, max_amount);

			create_tape(tmp_tapes[cur_tmp_amount], ("tmp\\tmpfile" + to_string(cur_tmp_amount) + ".txt"), max_amount, rw_delay, be_delay, shift_delay);
			copy_from_RAM(RAM, tmp_tapes[cur_tmp_amount], max_amount);
			cur_tmp_amount++;
		}

		if (N % max_amount != 0) {
			copy_in_RAM(tape_in, RAM, N % max_amount);
			sort(RAM, N % max_amount);

			create_tape(tmp_tapes[cur_tmp_amount], ("tmp\\tmpfile" + to_string(cur_tmp_amount) + ".txt"), N % max_amount, rw_delay, be_delay, shift_delay);
			copy_from_RAM(RAM, tmp_tapes[cur_tmp_amount], N % max_amount);
			cur_tmp_amount++;
		}

		merge(tmp_tapes, tmp_amount, tape_out, RAM);

		for (int i = 0; i < tmp_amount; i++) {
			remove(tmp_tapes[i].get_name().c_str());
		}

		delete[] tmp_tapes;
		delete[] RAM;
	}
};

int main()
{
	int N = 100;
	int M = 600;

	int rw_delay, be_delay, shift_delay;
	config(rw_delay, be_delay, shift_delay);

	string name_tape_in, name_tape_out;
	cin >> name_tape_in;
	cin >> name_tape_out;

	Tape tape_in(name_tape_in, N, rw_delay, be_delay, shift_delay);
	Tape tape_out(name_tape_out, N, rw_delay, be_delay, shift_delay);

	DoSort do_sort;
	do_sort.sort_tape(tape_in, tape_out, N, M);

}

void config(int &rw_delay, int &be_delay, int &shift_delay) {
	ifstream in;
	in.open("config.txt");
	if (in.is_open()) {
		in >> rw_delay;
		in >> be_delay;
		in >> shift_delay;
	}
	in.close();
}

void copy_in_RAM(Tape &tape, int* (&RAM), int N) {
	for (int i = 0; i < N; i++) {
		RAM[i] = tape.read();
		tape.shift(1);
	}
}

void copy_from_RAM(int* (&RAM), Tape &tape, int N) {
	for (int i = 0; i < N; i++) {
		tape.write(RAM[i]);
		tape.shift(1);
	}
}

void sort(int* (&arr), int N) {
	int temp;
	for (int i = 0; i < N - 1; i++) {
		for (int j = 0; j < N - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}

void create_tape(Tape &tmp_tape, string name, int length, int rw_d, int be_d, int shift_d) {
	tmp_tape.set_name(name);
	tmp_tape.set_length(length);
	tmp_tape.set_rw_delay(rw_d);
	tmp_tape.set_be_delay(be_d);
	tmp_tape.set_shift_delay(shift_d);
}

void merge(Tape* (&tmp_tapes), int tmp_amount, Tape &tape_out, int* (&RAM)) {

	for (int i = 0; i < tmp_amount; i++) {
		tmp_tapes[i].to_begin();
	}

	for (int i = 0; i < tmp_amount; i++) {
		RAM[i] = tmp_tapes[i].read();
		tmp_tapes[i].shift(1);
	}

	int min = RAM[0];
	int min_index = 0;

	int head_out;
	int head_tmp;

	do {

		int min = RAM[0];
		int min_index = 0;

		for (int i = 0; i < tmp_amount; i++) {
			if (RAM[i] < min) {
				min = RAM[i];
				min_index = i;
			}
		}
		tape_out.write(min);
		head_out = tape_out.shift(1);

		head_tmp = tmp_tapes[min_index].shift(0);
		if (head_tmp != -1) {
			RAM[min_index] = tmp_tapes[min_index].read();
			tmp_tapes[min_index].shift(1);
		}
		else {
			RAM[min_index] = 2147483647; // 2^31-1
		}

	} while (head_out != -1);

}