#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
using namespace std;

const char path[] = "./data/wave.dat";

class Wave {
    int id;
    char item[30];
    int qty;
    float weight;
    char priority;
    char zone[20];
    static int cnt;
public:
    static int show;

    // default constructor
    Wave() {
        id = 0;
        strcpy(item, "none");
        qty = 0;
        weight = 0;
        priority = 'L';
        strcpy(zone, "none");
        if (show) cout << "Default constructor called\n";
    }

    // parameterized constructor
    Wave(int i, const char *it, int q, float w, char p, const char *z) {
        id = i;
        strncpy(item, it, 29);
        item[29] = '\0';
        qty = q;
        weight = w;
        priority = p;
        strncpy(zone, z, 19);
        zone[19] = '\0';
        if (show) cout << "Parameterized constructor called\n";
    }

    // copy constructor
    Wave(const Wave &w) {
        id = w.id;
        strcpy(item, w.item);
        qty = w.qty;
        weight = w.weight;
        priority = w.priority;
        strcpy(zone, w.zone);
        if (show) cout << "Copy constructor called\n";
    }

    ~Wave() {
        if (show) cout << "Destructor called for id " << id << "\n";
    }

    int getId() { return id; }

    void setData(int id) {
        this->id = id;
        cin.ignore(1000, '\n');
        do {
            cout << "Item name: ";
            cin.getline(item, 30);
            if (!cin) { cin.clear(); cin.ignore(1000, '\n'); }
        } while (strlen(item) == 0);
        do {
            cout << "Quantity: ";
            cin >> qty;
            if (!cin) { cin.clear(); cin.ignore(1000, '\n'); qty = 0; }
        } while (qty <= 0);
        do {
            cout << "Weight: ";
            cin >> weight;
            if (!cin) { cin.clear(); cin.ignore(1000, '\n'); weight = 0; }
        } while (weight <= 0);
        do {
            cout << "Priority (H/M/L): ";
            cin >> priority;
            priority = toupper(priority);
        } while (priority != 'H' && priority != 'M' && priority != 'L');
        cin.ignore(1000, '\n');
        do {
            cout << "Zone: ";
            cin.getline(zone, 20);
            if (!cin) { cin.clear(); cin.ignore(1000, '\n'); }
        } while (strlen(zone) == 0);
    }

    inline void display() {
        cout << id << "\t" << item << "\t" << qty << "\t" << weight << "\t" << priority << "\t" << zone << "\n";
    }

    static int getCount() { return cnt; }

    // cnt = file size / size of one record
    static void loadCount() {
        fstream file(path, ios::binary | ios::in);
        file.seekg(0, ios::end);
        int bytes = file.tellg();
        if (bytes < 0) bytes = 0;
        cnt = bytes / (int)sizeof(Wave);
        file.close();
    }

    friend void showPriority(Wave w);
};

int Wave::cnt = 0;
int Wave::show = 0;

void showPriority(Wave w) {
    cout << "ID " << w.id << " has priority " << w.priority << "\n";
}

// returns record number (0,1,2..) of the id, -1 if not there
int findPos(int id) {
    fstream file(path, ios::binary | ios::in);
    Wave temp;
    int pos = 0, found = -1;
    while (file.read((char*)&temp, sizeof(Wave))) {
        if (temp.getId() == id) {
            found = pos;
            break;
        }
        pos++;
    }
    file.close();
    return found;
}

void create() {
    Wave temp;
    int id;
    cout << "Enter ID: ";
    cin >> id;
    if (!cin || id <= 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "ID must be a positive number\n";
        return;
    }
    if (findPos(id) != -1) {
        cout << "Duplicate ID, record not saved\n";
        return;
    }
    temp.setData(id);
    fstream file(path, ios::binary | ios::out | ios::app);
    file.write((char*)&temp, sizeof(Wave));
    file.close();
    Wave::loadCount();
    cout << "Record saved\n";
}

void readAll() {
    fstream file(path, ios::binary | ios::in);
    Wave temp;
    int n = 0;
    cout << "ID\tItem\tQty\tWeight\tPri\tZone\n";
    while (file.read((char*)&temp, sizeof(Wave))) {
        temp.display();
        n++;
    }
    file.close();
    if (n == 0)
        cout << "No records in file\n";
}

void search() {
    Wave temp;
    int id, pos;
    cout << "Enter ID to search: ";
    cin >> id;
    pos = findPos(id);
    if (pos == -1) {
        cout << "Record not found\n";
        return;
    }
    fstream file(path, ios::binary | ios::in);
    file.seekg(pos * sizeof(Wave), ios::beg);
    file.read((char*)&temp, sizeof(Wave));
    file.close();
    cout << "Record number " << pos + 1 << ", starts at byte " << pos * sizeof(Wave) << "\n";
    cout << "ID\tItem\tQty\tWeight\tPri\tZone\n";
    temp.display();
}

void update() {
    Wave old, newOne;
    int id, pos;
    cout << "Enter ID to update: ";
    cin >> id;
    pos = findPos(id);
    if (pos == -1) {
        cout << "Record not found\n";
        return;
    }
    fstream file(path, ios::binary | ios::in | ios::out);
    file.seekg(pos * sizeof(Wave), ios::beg);
    file.read((char*)&old, sizeof(Wave));
    cout << "Old record:\n";
    old.display();
    cout << "Enter new details (ID stays same)\n";
    newOne.setData(id);
    file.seekp(pos * sizeof(Wave), ios::beg);
    file.write((char*)&newOne, sizeof(Wave));
    file.close();
    cout << "Record updated\n";
}

void demo() {
    Wave::show = 1;
    {
        cout << "Making a (no values given)\n";
        Wave a;
        cout << "Making b (values given)\n";
        Wave b(101, "Laptop", 5, 2.5, 'H', "A1");
        cout << "Making c from b\n";
        Wave c = b;
        cout << "Calling showPriority(b), object goes by value\n";
        showPriority(b);
        cout << "End of block\n";
    }
    Wave::show = 0;
}

int main() {
    int choice = 0;

    // make sure the file exists
    fstream f(path, ios::binary | ios::out | ios::app);
    if (!f) {
        cout << "Cannot open " << path << ", create the data folder first\n";
        return 0;
    }
    f.close();
    Wave::loadCount();

    while (choice != 6) {
        cout << "\n--- Warehouse Picking Wave Planner ---\n";
        cout << "Records in file: " << Wave::getCount() << "\n";
        cout << "1. Create\n2. Read all\n3. Search\n4. Update\n5. Constructor demo\n6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        if (!cin) {
            cin.clear();
            cin.ignore(1000, '\n');
            choice = 0;
        }
        switch (choice) {
            case 1: create(); break;
            case 2: readAll(); break;
            case 3: search(); break;
            case 4: update(); break;
            case 5: demo(); break;
            case 6: cout << "Bye\n"; break;
            default: cout << "Wrong choice\n";
        }
    }
    return 0;
}
