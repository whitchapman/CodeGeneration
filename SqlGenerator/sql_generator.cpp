#include <iostream>
#include <vector>
#include <map>

using namespace std;

//////////////////////////////////////////////

class Buffer {
	string line;
	char buffer[200];
	char *ptr;
public:
	Buffer();
	void read_line();
	char *extract_param();
};

Buffer::Buffer() {
}

void Buffer::read_line() {
	getline(cin, line);
	strcpy(buffer, line.c_str());
	ptr = buffer;
}

char *Buffer::extract_param() {
	char *retval = ptr;

	char *next = strchr(ptr, ' ');
	if (NULL != next) {
		*next = '\0';
		next++;
	}

	ptr = next;
	return retval;
}

//////////////////////////////////////////////


//////////////////////////////////////////////

class Table {
	string _entity_name;
	string _table_name;
	string _table_id;

	Table *_one_to_one;
	vector<Table*> _one_to_many;
public:
	Table(char*);
	string get_entity_name();
	string get_table_name();
	string get_table_id();

	void set_one_to_one(Table*);
	Table *get_one_to_one();

	void add_one_to_many(Table*);
	vector<Table*> &get_one_to_many();
};

Table::Table(char *s) {
	_entity_name = s;
	_table_name = s + string("s");
	_table_id = s + string("_id");

	_one_to_one = NULL;
}

string Table::get_entity_name() {
	return _entity_name;
}

string Table::get_table_name() {
	return _table_name;
}

string Table::get_table_id() {
	return _table_id;
}

void Table::set_one_to_one(Table *t) {
	_one_to_one = t;
}

Table *Table::get_one_to_one() {
	return _one_to_one;
}

void Table::add_one_to_many(Table *t) {
	_one_to_many.push_back(t);
}

vector<Table*> &Table::get_one_to_many() {
	return _one_to_many;
}

/////////////////////////

typedef map<string, Table*>::iterator table_iterator_type;

bool setup(vector<Table*> &tables, map<string, Table*> &table_map) {

	bool retval = false;
	Buffer *buffer = new Buffer();

	bool done = false;
	while (!done) {
		buffer->read_line();
		char *param = buffer->extract_param();
		if (strlen(param) == 0) {
			retval = true;
			done = true;
		} else {

			bool success = true;
			if (param[0] == 'E') {

				param = buffer->extract_param();

				Table *t = new Table(param);
				tables.push_back(t);
				table_map[t->get_entity_name()] = t;

			} else if (param[0] == 'R') {

				string param1 = buffer->extract_param();
				string param2 = buffer->extract_param();
				char *param3 = buffer->extract_param();

				Table *parent = table_map[param1];
				if (parent != NULL) {
					Table *child = table_map[param2];
					if (child != NULL) {
						if (param3[0] == '1') {
							child->set_one_to_one(parent);
						} else if (param3[0] == 'N') {
							child->add_one_to_many(parent);
						} else {
							cout << "INVALID RELATION: " << param3 << endl;
							done = true;
						}
					} else {
						cout << "INVALID CHILD TABLE: " << param2 << endl;
						done = true;
					}
				} else {
					cout << "INVALID PARENT TABLE: " << param1 << endl;
					done = true;
				}

			} else {
				cout << "INVALID TYPE: " << param << endl;
				done = true;
			}
		}
	}

	delete buffer;
	return retval;
}

void write_table_schemas(vector<Table*> &tables) {

	cout << endl;
	for (int i=0; i < tables.size(); i++) {

		Table *t = tables[i];
		Table *one = t->get_one_to_one();
		vector<Table*> manys = t->get_one_to_many();

		//open create table statement
		cout << "create table " << t->get_table_name() << " (" << endl;
		if (one != NULL) {
			cout << "  " << one->get_table_id() << " int unsigned not null," << endl;;
		} else {
		 	cout << "  " << t->get_table_id() << " int unsigned not null auto_increment," << endl;
		}

		//timestamp fields
  	cout << "  create_time timestamp null, #must allow null in order for last_updated column to work correctly" << endl;
  	cout << "  last_updated timestamp not null default current_timestamp on update current_timestamp," << endl;

  	//foreign key fields
		for (int i=0; i < manys.size(); i++) {
			Table *many = manys[i];
			cout << "  " << many->get_table_id() << " int unsigned not null," << endl;
		}

		//table specific fields



		//primary and foreign key definitions
		if (one != NULL) {
	 	 	cout << "  primary key (" << one->get_table_id() << ")," << endl;
	 	 	cout << "  foreign key (" << one->get_table_id() << ") references " << one->get_table_name() << "(" << one->get_table_id() << ")";
		} else {
	 	 	cout << "  primary key (" << t->get_table_id() << ")";
		}

		for (int i=0; i < manys.size(); i++) {
			Table *many = manys[i];
 	 		cout << "," << endl;
	 	 	cout << "  foreign key (" << many->get_table_id() << ") references " << many->get_table_name() << "(" << many->get_table_id() << ")";
		}
 	 	cout << endl;

 	 	//close create table statement
		cout << ") default character set utf8;" << endl;
		cout << endl;
	}
}

void cleanup(map<string, Table*> &table_map) {
	for (table_iterator_type i = table_map.begin(); i != table_map.end(); i++) {
		delete i->second;
	}	
}

int main() {

	vector<Table*> tables;
	map<string, Table*> table_map;

	setup(tables, table_map);
	write_table_schemas(tables);
	cleanup(table_map);

	return 0;
}
