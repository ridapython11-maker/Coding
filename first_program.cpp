#include <iostream>
#include <string>
using namespace std;
struct emp {
	int id;
	string name;
	string des;
};
struct dep {
	string name;
	string manager;
	emp e1[3];
};
dep d1[3];
void output() {
	for (int i = 0; i < 3; i++) {
		cout << d1[i].name << endl;
		cout << d1[i].manager << endl;
		for (int j = 0;j < 3; j++) {
			cout << d1[i].e1[j].id << endl;
			cout << d1[i].e1[j].name << endl;
			cout << d1[i].e1[j].des << endl;
		}
	}
}
void main() {
	for (int i = 0; i < 3;i++) {
		cout << "department " << i + 1 << " :- " << endl;
		cout << "name: ";
		cin >> d1[i].name;
		cout << "manager: ";
		cin >> d1[i].manager;
		for (int j = 0; j < 3; j++) {
			cout << "employee " << j + 1 << " :- " << endl;
			cout << "enter id: ";
			cin >> d1[i].e1[j].id;
			cout << "enter name: ";
			cin >> d1[i].e1[j].name;
			cout << "enter designation: ";
			cin >> d1[i].e1[j].des;
		}
	}
	output();
	system("pause");
}

