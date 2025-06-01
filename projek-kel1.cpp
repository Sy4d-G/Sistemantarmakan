#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <queue>
#include <utility>
#include <thread>
#include <chrono>
#include <map>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <tuple>
#include <conio.h>
#include <cctype>

using namespace std;

struct TitikPeta {
    int id;
    string nama;
    string tipe;
};

// <<< STRUCT ITEMMENU DIPERBARUI >>>
struct ItemMenu {
    int id;
    string nama;
    double harga;
    string kategori;
    string emoticon; // Tambahan emoticon
};
// <<< AKHIR STRUCT ITEMMENU DIPERBARUI >>>

struct Restoran {
    int idTitik;
    string nama;
    vector<ItemMenu> menu;
};

struct Pengguna {
    string username;
    string password;
    bool punyaVoucher = true;
};

struct Pesanan {
    string username;
    int idRestoran;
    vector<tuple<ItemMenu, int, string>> daftarItem;
    string catatanPesananUmum = "";
    int idTitikTujuan;
    double totalHarga = 0.0;
    double jarakAntar = 0.0;
    double biayaAntar = 0.0;
    bool pakaiVoucher = false;
    bool pakaiSedotan = false;
    string metodeAmbil;
    string tipeLayananDriver = "Reguler";
};

map<string, Pengguna> daftarPengguna;
vector<Restoran> daftarRestoran;
vector<TitikPeta> daftarTitikPeta;
vector<vector<pair<int, double>>> daftarKoneksi;

void bersihkanLayar() {
    // Untuk Windows, coba juga system("cls"); jika \033[2J... tidak bekerja dengan baik
    // Untuk Linux/macOS, coba juga system("clear");
    cout << "\033[2J\033[1;1H"; // Ini adalah ANSI escape code, mungkin perlu pengaturan terminal
}

void animasiLoading(const string& pesan, int detik) {
    cout << pesan;
    for (int i = 0; i < detik * 2; ++i) {
        cout << "." << flush;
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    cout << endl;
}

void cetakSelamatDatang() {
    bersihkanLayar();
    cout << "========================================" << endl;
    cout << "      SELAMAT DATANG DI GO-FOOD C++      " << endl;
    cout << "========================================" << endl;
    animasiLoading("Memuat aplikasi", 2);
}

void buatPeta() {
    daftarTitikPeta = {
        {0, "RM Padang", "Restoran"}, {1, "McDonald's", "Restoran"},
        {2, "Mie Gacoan", "Restoran"}, {3, "Ayam Sriwedari", "Restoran"},
        {4, "Janji Jiwa", "Restoran"}, {5, "Kos Sabila", "Pelanggan"},
        {6, "Kos Bintang", "Pelanggan"}, {7, "Kos Jansen", "Pelanggan"},
        {8, "Simpang UNSIKA", "Persimpangan"}, {9, "Bundaran KCP", "Persimpangan"},
        {10, "Perempatan L.Merah", "Persimpangan"}
    };
    daftarKoneksi.resize(11);
    auto tambahJalan = [&](int u, int v, double w) {
        daftarKoneksi[u].push_back({v, w});
        daftarKoneksi[v].push_back({u, w});
    };
    tambahJalan(0, 10, 3.0); tambahJalan(1, 9, 0.8); tambahJalan(2, 8, 0.5);
    tambahJalan(3, 10, 2.5); tambahJalan(4, 8, 0.4); tambahJalan(5, 8, 0.7);
    tambahJalan(6, 10, 1.7); tambahJalan(7, 10, 2.1); tambahJalan(8, 9, 3.5);
    tambahJalan(9, 10, 2.8); tambahJalan(8, 10, 4.2);
}

void tampilkanPetaUntukPilihan() {
    cout << "======================================" << endl;
    cout << "  Pilih Lokasi Tujuan Anda" << endl;
    cout << "======================================" << endl;
    cout << "--- Peta Jaringan (Sketsa Kasar) ---" << endl;
    cout << "  [R2:2]--0.5--[J0:8]--0.4--[R4:4]" << endl;
    cout << "               |    \\" << endl;
    cout << "             0.7|     \\ 3.5" << endl;
    cout << "               |      \\" << endl;
    cout << "           [P0:5]      [J1:9]--0.8--[R1:1]" << endl;
    cout << "                       / |" << endl;
    cout << "                  4.2 /  | 2.8" << endl;
    cout << "                     /   |" << endl;
    cout << "  [R0:0]--3.0--[J2:10]--2.5--[R3:3]" << endl;
    cout << "               |    |" << endl;
    cout << "             1.7|    | 2.1" << endl;
    cout << "               |    |" << endl;
    cout << "           [P1:6]  [P2:7]" << endl;
    cout << "--------------------------------------" << endl;
    cout << "--- Jaringan Peta & Jarak (km) ---" << endl;
    cout << fixed << setprecision(1);
    for (int i = 0; i < daftarTitikPeta.size(); ++i) {
        cout << "[" << setw(2) << daftarTitikPeta[i].id << ": " << daftarTitikPeta[i].nama << "] -> { ";
        bool first = true;
        for (const auto& edge : daftarKoneksi[i]) {
            if (!first) { cout << ", "; }
            cout << edge.first << " (" << edge.second << ")";
            first = false;
        }
        cout << " }" << endl;
    }
     cout << "--------------------------------------" << endl;
}

double hitungJarakTerpendek(int startNode, int endNode) {
    int n = daftarTitikPeta.size();
    vector<double> dist(n, numeric_limits<double>::max());
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    dist[startNode] = 0.0;
    pq.push({0.0, startNode});

    while (!pq.empty()) {
        double d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (d > dist[u]) continue;
        if (u == endNode) return dist[endNode];

        for (const auto& edge : daftarKoneksi[u]) {
            int v = edge.first;
            double weight = edge.second;
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }
    return numeric_limits<double>::max();
}

string dapatkanPasswordTersembunyiWindows() {
    string password = "";
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b' || ch == 8) {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else if (isprint(ch)) {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

// <<< FUNGSI YANG DIPERBARUI >>>
void isiMenu() {
    // Emoticon: 🥘 (Padang), 🍔 (Burger), 🍜 (Mie), 🍗 (Ayam), ☕️ (Kopi)
    // 🥤 (Minuman Gelas), 🧋 (Bubble Tea/Teh Botol), 🍟 (Kentang), 🍞 (Roti), 🍫 (Coklat)
    // 🌶️ (Pedas), 🧊 (Es), 🧀 (Keju), 🍤 (Udang)
    // Pastikan terminal Anda mendukung Unicode & font yang sesuai.
    daftarRestoran.push_back({0, "RM Padang", {
        {101, "Nasi Rendang", 25000, "Makanan", "🥘"},
        {102, "Nasi Ayam Pop", 23000, "Makanan", "🍗"},
        {103, "Teh Botol", 5000, "Minuman", "🧋"},
        {104, "Teh Botol Low Sugar", 5500, "Minuman", "🧋"}
    }});
    daftarRestoran.push_back({1, "McDonald's", {
        {201, "Big Mac", 35000, "Makanan", "🍔"},
        {202, "French Fries", 15000, "Makanan", "🍟"},
        {203, "Coca Cola", 10000, "Minuman", "🥤"}
    }});
     daftarRestoran.push_back({2, "Mie Gacoan", {
        {301, "Mie Gacoan", 12000, "Makanan", "🍜🌶️"},
        {302, "Udang Keju", 9000, "Makanan", "🍤🧀"},
        {303, "Es Gobak Sodor", 10000, "Minuman", "🍹🧊"}
    }});
     daftarRestoran.push_back({3, "Ayam Sriwedari", {
        {401, "Paket Ayam Bakar", 28000, "Makanan", "🍗🔥"},
        {402, "Tahu/Tempe", 3000, "Makanan", "🍲"}, // 🍲 (Pot of food) for generic side
        {403, "Es Teh Manis", 6000, "Minuman", "🍹🧊"}
    }});
     daftarRestoran.push_back({4, "Janji Jiwa", {
        {501, "Kopi Susu", 18000, "Minuman", "☕️"},
        {502, "Toast Coklat", 22000, "Makanan", "🍞🍫"},
        {503, "Matcha Latte", 24000, "Minuman", "🍵"},
        {504, "Kopi Sahabat", 13000, "Minuman", "☕️"}
    }});
}
// <<< AKHIR FUNGSI YANG DIPERBARUI >>>


Restoran* cariRestoranById(int idTitik) {
    for (auto& resto : daftarRestoran) {
        if (resto.idTitik == idTitik) {
            return &resto;
        }
    }
    return nullptr;
}

void daftarPenggunaBaru() {
    bersihkanLayar();
    Pengguna penggunaBaru;
    cout << "--- DAFTAR PENGGUNA BARU ---" << endl;
    cout << "Masukkan Username Baru: ";
    cin >> penggunaBaru.username;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (daftarPengguna.find(penggunaBaru.username) != daftarPengguna.end()) {
        cout << "Username sudah ada. Silakan login atau gunakan username lain." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        return;
    }
    cout << "Masukkan Password Baru: ";
    penggunaBaru.password = dapatkanPasswordTersembunyiWindows();
    daftarPengguna[penggunaBaru.username] = penggunaBaru;
    cout << "Registrasi Berhasil! Anda mendapatkan 1 voucher diskon 5000." << endl;
    this_thread::sleep_for(chrono::seconds(2));
}

Pengguna* masukPengguna() {
    bersihkanLayar();
    string username, password_input;
    cout << "--- LOGIN PENGGUNA ---" << endl;
    cout << "Masukkan Username: ";
    cin >> username;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Masukkan Password: ";
    password_input = dapatkanPasswordTersembunyiWindows();
    auto it = daftarPengguna.find(username);
    if (it != daftarPengguna.end() && it->second.password == password_input) {
        cout << "Login Berhasil!" << endl;
        this_thread::sleep_for(chrono::seconds(1));
        return &(it->second);
    } else {
        cout << "Username atau Password Salah." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        return nullptr;
    }
}

Pengguna* aturOtentikasi() {
    int pilihan;
    Pengguna* penggunaSaatIni = nullptr;
    while (!penggunaSaatIni) {
        bersihkanLayar();
        cout << "========================" << endl;
        cout << "      MENU UTAMA" << endl;
        cout << "========================" << endl;
        cout << "1. Login" << endl;
        cout << "2. Daftar" << endl;
        cout << "3. Keluar" << endl;
        cout << "Pilihan Anda: ";
        cin >> pilihan;
        if (cin.fail()) {
            cout << "Input tidak valid! Harap masukkan angka." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (pilihan) {
            case 1:
                penggunaSaatIni = masukPengguna();
                break;
            case 2:
                daftarPenggunaBaru();
                break;
            case 3:
                return nullptr;
            default:
                cout << "Pilihan tidak valid." << endl;
                this_thread::sleep_for(chrono::seconds(1));
                break;
        }
    }
    return penggunaSaatIni;
}

Restoran* pilihRestoran() {
    int pilihan;
    while(true) {
        bersihkanLayar();
        cout << "======================================" << endl;
        cout << "     Pilih Restoran Favorit Anda" << endl;
        cout << "======================================" << endl;
        for (const auto& resto : daftarRestoran) {
            cout << resto.idTitik + 1 << ". " << resto.nama
                      << " (" << daftarTitikPeta[resto.idTitik].nama << ")" << endl;
        }
        cout << "Masukkan Nomor Restoran Pilihan Anda: ";
        cin >> pilihan;
        if (cin.fail()) {
            cout << "Input tidak valid! Harap masukkan angka." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (pilihan > 0 && pilihan <= daftarRestoran.size()) {
            return cariRestoranById(pilihan - 1);
        } else {
            cout << "Pilihan tidak valid!" << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
}

// <<< FUNGSI YANG DIPERBARUI >>>
void pilihMenu(Restoran* restoTerpilih, Pesanan& pesananSaatIni) {
    int id_menu, jumlah;
    char tambahLagi;

    do {
        bersihkanLayar();
        cout << "--- Menu " << restoTerpilih->nama << " ---" << endl;
        for (const auto& item : restoTerpilih->menu) {
            // Tampilkan emoticon bersama nama menu
            cout << item.id << ". " << item.emoticon << " " << item.nama << " - Rp " << item.harga
                      << " (" << item.kategori << ")" << endl;
        }
        cout << "-------------------------" << endl;
        cout << "Masukkan ID Menu (0 untuk selesai): ";
        cin >> id_menu;

        if (cin.fail() || id_menu == 0) {
             if(cin.fail()){
                cout << "Input tidak valid!" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                this_thread::sleep_for(chrono::seconds(1));
             }
             break;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Masukkan Jumlah: ";
        cin >> jumlah;

         if (cin.fail() || jumlah <= 0) {
             cout << "Jumlah tidak valid!" << endl;
             cin.clear();
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
             this_thread::sleep_for(chrono::seconds(1));
             continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        bool ditemukan = false;
        for (const auto& item : restoTerpilih->menu) {
            if (item.id == id_menu) {
                double hargaItemSaatIni = item.harga;
                string catatanKhusus = "";

                if (restoTerpilih->nama == "Mie Gacoan" && item.nama == "Mie Gacoan") {
                    int levelPedas;
                    cout << "Pilih level pedas untuk " << item.nama << " (1-10): ";
                    cin >> levelPedas;
                    if(cin.fail() || levelPedas < 1 || levelPedas > 10){
                        cout << "Level pedas tidak valid, diatur ke level 1." << endl;
                        levelPedas = 1;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    catatanKhusus = "Level " + to_string(levelPedas);
                    if (levelPedas >= 6) {
                        hargaItemSaatIni += 2500;
                    }
                }
                else if (restoTerpilih->nama == "Ayam Sriwedari" && item.nama == "Paket Ayam Bakar") {
                    int levelKepedasan;
                    cout << "Pilih level kepedasan untuk Paket Ayam Bakar (1-4): ";
                    cin >> levelKepedasan;
                     if(cin.fail() || levelKepedasan < 1 || levelKepedasan > 4){
                        cout << "Level kepedasan tidak valid, diatur ke level 1." << endl;
                        levelKepedasan = 1;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    catatanKhusus = "Pedas Level " + to_string(levelKepedasan);
                }
                // Kustomisasi untuk minuman di Ayam Sriwedari, Janji Jiwa, Mie Gacoan (untuk es)
                if (item.kategori == "Minuman") {
                    if (restoTerpilih->nama == "Ayam Sriwedari" && item.nama == "Es Teh Manis") {
                        int opsiGula;
                        cout << "Pilih opsi gula untuk Es Teh Manis (1: Regular, 2: Less Sugar): ";
                        cin >> opsiGula;
                        if(cin.fail() || (opsiGula != 1 && opsiGula !=2) ){
                            cout << "Opsi gula tidak valid, diatur ke Regular." << endl;
                            opsiGula = 1;
                        }
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        catatanKhusus = (opsiGula == 2) ? "Less Sugar" : "Regular";
                    }
                    else if (restoTerpilih->nama == "Janji Jiwa" &&
                             (item.nama == "Kopi Susu" || item.nama == "Matcha Latte" || item.nama == "Kopi Sahabat")) {
                        int opsiGula;
                        cout << "Pilih opsi gula untuk " << item.nama << " (1: Regular, 2: Less Sugar): ";
                        cin >> opsiGula;
                         if(cin.fail() || (opsiGula != 1 && opsiGula !=2) ){
                            cout << "Opsi gula tidak valid, diatur ke Regular." << endl;
                            opsiGula = 1;
                        }
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        catatanKhusus = (opsiGula == 2) ? "Less Sugar" : "Regular";
                    }

                    // Opsi Es Umum untuk minuman yang memenuhi syarat
                    if (restoTerpilih->nama == "Ayam Sriwedari" || restoTerpilih->nama == "Janji Jiwa" || restoTerpilih->nama == "Mie Gacoan") {
                         int opsiEs;
                         cout << "Pilih opsi es untuk " << item.nama << " (1: Regular Ice, 2: Less Ice): ";
                         cin >> opsiEs;
                         if(cin.fail() || (opsiEs != 1 && opsiEs !=2) ){
                             cout << "Opsi es tidak valid, diatur ke Regular Ice." << endl;
                             opsiEs = 1;
                         }
                         cin.ignore(numeric_limits<streamsize>::max(), '\n');
                         if (!catatanKhusus.empty()) catatanKhusus += ", ";
                         catatanKhusus += (opsiEs == 2) ? "Less Ice" : "Regular Ice";
                    }
                }


                pesananSaatIni.daftarItem.emplace_back(item, jumlah, catatanKhusus);
                pesananSaatIni.totalHarga += hargaItemSaatIni * jumlah;
                cout << item.emoticon << " " << item.nama << (catatanKhusus.empty() ? "" : " (" + catatanKhusus + ")") << " x " << jumlah << " ditambahkan ke keranjang." << endl;
                ditemukan = true;
                this_thread::sleep_for(chrono::seconds(1));
                break;
            }
        }
        if (!ditemukan) {
            cout << "ID Menu tidak ditemukan." << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }

        cout << "Tambah pesanan lagi? (y/n): ";
        cin >> tambahLagi;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

    } while (tambahLagi == 'y' || tambahLagi == 'Y');

    char tambahCatatanUmum;
    cout << "Apakah ada catatan khusus untuk seluruh pesanan makanan Anda? (y/n): ";
    cin >> tambahCatatanUmum;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (tambahCatatanUmum == 'y' || tambahCatatanUmum == 'Y') {
        cout << "Masukkan catatan Anda: ";
        getline(cin, pesananSaatIni.catatanPesananUmum);
    }

    bool keranjangAdaMinuman = false;
    for(const auto& pesanan_tuple : pesananSaatIni.daftarItem){
        if(get<0>(pesanan_tuple).kategori == "Minuman"){
            keranjangAdaMinuman = true;
            break;
        }
    }

    if (keranjangAdaMinuman) {
        char pilihanSedotan;
        cout << "Apakah Anda ingin menambah sedotan karton (Rp 2000)? (y/n): ";
        cin >> pilihanSedotan;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (pilihanSedotan == 'y' || pilihanSedotan == 'Y') {
            pesananSaatIni.pakaiSedotan = true;
        }
    }
}
// <<< AKHIR FUNGSI YANG DIPERBARUI >>>


void gunakanVoucher(Pengguna* penggunaAktif, Pesanan& pesananSaatIni) {
     if (penggunaAktif->punyaVoucher) {
        char pilihanVoucher;
        cout << "Anda memiliki 1 voucher diskon Rp 5000. Gunakan? (y/n): ";
        cin >> pilihanVoucher;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (pilihanVoucher == 'y' || pilihanVoucher == 'Y') {
            pesananSaatIni.pakaiVoucher = true;
            penggunaAktif->punyaVoucher = false;
            cout << "Voucher berhasil digunakan!" << endl;
        }
    }
}

int pilihTujuan(int idTitikRestoran) {
    int pilihan;
    while (true) {
        bersihkanLayar();
        tampilkanPetaUntukPilihan();
        cout << "Masukkan Nomor Lokasi Tujuan Anda (0-10): ";
        cin >> pilihan;

        if (cin.fail()) {
             cout << "Input tidak valid! Harap masukkan angka." << endl;
             cin.clear();
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
             this_thread::sleep_for(chrono::seconds(1));
        } else if (pilihan >= 0 && pilihan < daftarTitikPeta.size()) {
            return pilihan;
        } else {
            cout << "Pilihan tidak valid!" << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
}

void simulasiPengantaran(double totalHargaAkhir, double jarak) {
    animasiLoading("Sedang mencari driver", 3);
    cout << "Driver telah ditemukan, sedang mengambil pesanan anda" << endl;
    this_thread::sleep_for(chrono::seconds(4));
    cout << "Driver telah meng-pickup pesanan anda, driver sedang menuju ke lokasi anda berada (" << fixed << setprecision(1) << jarak << " km)" << endl;
    this_thread::sleep_for(chrono::seconds(5));
    cout << "Driver telah sampai di titik yang ditentukan, silahkan siapkan uang sebesar Rp "
              << fixed << setprecision(0) << totalHargaAkhir << endl;
    this_thread::sleep_for(chrono::seconds(3));
}

// <<< FUNGSI YANG DIPERBARUI >>>
void prosesPesanan(Pengguna* penggunaAktif, Pesanan& pesananSaatIni) {
    bersihkanLayar();
    cout << "--- Rincian Pesanan ---" << endl;
    double subTotalItem = 0;

    string namaRestoranPesanan = "";
    for(const auto& r : daftarRestoran) {
        if (r.idTitik == pesananSaatIni.idRestoran) {
            namaRestoranPesanan = r.nama;
            break;
        }
    }

    for(const auto& pesanan_tuple : pesananSaatIni.daftarItem) {
        const ItemMenu& item = get<0>(pesanan_tuple);
        int kuantitas = get<1>(pesanan_tuple);
        const string& catatan = get<2>(pesanan_tuple);
        double hargaPerItem = item.harga;

        if (namaRestoranPesanan == "Mie Gacoan" && item.nama == "Mie Gacoan") {
            if (catatan.find("Level ") != string::npos) {
                try {
                    size_t levelPos = catatan.find("Level ");
                    if (levelPos != string::npos) {
                        string levelStr = catatan.substr(levelPos + 6); // Ambil bagian angka saja
                        // Hapus bagian ", Less Ice" atau ", Regular Ice" jika ada
                        size_t commaPos = levelStr.find(",");
                        if (commaPos != string::npos) {
                            levelStr = levelStr.substr(0, commaPos);
                        }
                        if (!levelStr.empty()) {
                             int level = stoi(levelStr);
                             if (level >= 6) {
                                 hargaPerItem += 2500;
                             }
                        }
                    }
                } catch (const std::invalid_argument& ia) {}
                  catch (const std::out_of_range& oor) {}
            }
        }
        // Tampilkan emoticon di rincian pesanan
        cout << item.emoticon << " " << item.nama << (catatan.empty() ? "" : " (" + catatan + ")") << " x " << kuantitas << " = Rp "
             << fixed << setprecision(0) << hargaPerItem * kuantitas << endl;
        subTotalItem += hargaPerItem * kuantitas;
    }
    pesananSaatIni.totalHarga = subTotalItem;

    if (pesananSaatIni.pakaiSedotan) {
         cout << "Sedotan Karton = Rp 2000" << endl;
         pesananSaatIni.totalHarga += 2000;
    }
    if (!pesananSaatIni.catatanPesananUmum.empty()){
        cout << "Catatan Umum: " << pesananSaatIni.catatanPesananUmum << endl;
    }
    
    cout << "-------------------------" << endl;
    cout << "Subtotal Pesanan: Rp " << fixed << setprecision(0) << pesananSaatIni.totalHarga << endl << endl;

    gunakanVoucher(penggunaAktif, pesananSaatIni);

    if(pesananSaatIni.pakaiVoucher) {
        pesananSaatIni.totalHarga -= 5000;
        if(pesananSaatIni.totalHarga < 0) pesananSaatIni.totalHarga = 0;
    }

    cout << "Harga Setelah Opsi Tambahan & Voucher: Rp " << fixed << setprecision(0) << pesananSaatIni.totalHarga << endl << endl;

    int tujuan = pilihTujuan(pesananSaatIni.idRestoran);
    pesananSaatIni.idTitikTujuan = tujuan;

    if (tujuan == pesananSaatIni.idRestoran) {
        pesananSaatIni.metodeAmbil = "Pick Up";
        cout << "Anda memilih Ambil Sendiri (Pick Up)." << endl;
        cout << "Total Bayar: Rp " << fixed << setprecision(0) << pesananSaatIni.totalHarga << endl;
        cout << "Pesanan akan siap dalam 15 menit. Silakan ditunggu." << endl;
        this_thread::sleep_for(chrono::seconds(5));
    } else {
        pesananSaatIni.metodeAmbil = "Antar";
        cout << "Anda memilih Antar ke Lokasi." << endl;

        pesananSaatIni.jarakAntar = hitungJarakTerpendek(pesananSaatIni.idRestoran, tujuan);
        if(pesananSaatIni.jarakAntar == numeric_limits<double>::max()){
            cout << "Maaf, tidak ada rute ke lokasi tujuan." << endl;
            pesananSaatIni.metodeAmbil = "Batal";
            this_thread::sleep_for(chrono::seconds(3));
            return;
        }
        
        double biayaAntarDasar = pesananSaatIni.jarakAntar * 1500;
        pesananSaatIni.biayaAntar = biayaAntarDasar;

        int pilihanLayanan;
        cout << "Pilih tipe layanan driver:" << endl;
        cout << "1. Reguler (Biaya Dasar: Rp " << fixed << setprecision(0) << biayaAntarDasar << ")" << endl;
        cout << "2. Express (+Rp 3000 dari biaya Reguler)" << endl;
        cout << "Pilihan Anda (1/2): ";
        cin >> pilihanLayanan;
        if (cin.fail() || (pilihanLayanan != 1 && pilihanLayanan != 2)) {
            cout << "Pilihan layanan tidak valid, dipilih Reguler." << endl;
            pilihanLayanan = 1;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');


        if (pilihanLayanan == 2) {
            pesananSaatIni.biayaAntar += 3000;
            pesananSaatIni.tipeLayananDriver = "Express";
        } else {
            pesananSaatIni.tipeLayananDriver = "Reguler";
        }
        
        pesananSaatIni.totalHarga += pesananSaatIni.biayaAntar;

        cout << "Jarak Pengantaran: " << fixed << setprecision(1) << pesananSaatIni.jarakAntar << " km" << endl;
        cout << "Tipe Layanan: " << pesananSaatIni.tipeLayananDriver << endl;
        cout << "Biaya Antar: Rp " << fixed << setprecision(0) << pesananSaatIni.biayaAntar << endl;
        cout << "-------------------------" << endl;
        cout << "Total Akhir (Termasuk Ongkir): Rp " << fixed << setprecision(0) << pesananSaatIni.totalHarga << endl;
        this_thread::sleep_for(chrono::seconds(2));

        simulasiPengantaran(pesananSaatIni.totalHarga, pesananSaatIni.jarakAntar);
    }
}
// <<< AKHIR FUNGSI YANG DIPERBARUI >>>


int main() {
    srand(time(0));
    buatPeta();
    isiMenu();

    cetakSelamatDatang();

    Pengguna* penggunaAktif = aturOtentikasi();
    if (!penggunaAktif) {
        cout << "Keluar dari program." << endl;
        return 0;
    }

    char pesanLagi;
    do {
        bersihkanLayar();

        Restoran* restoTerpilih = pilihRestoran();
        if (!restoTerpilih) {
            cout << "Terjadi kesalahan pemilihan restoran." << endl;
            return 1;
        }

        Pesanan pesananSaatIni;
        pesananSaatIni.username = penggunaAktif->username;
        pesananSaatIni.idRestoran = restoTerpilih->idTitik;

        pilihMenu(restoTerpilih, pesananSaatIni);

        if(pesananSaatIni.daftarItem.empty()) {
            cout << "Anda tidak memesan apapun." << endl;
        } else {
            prosesPesanan(penggunaAktif, pesananSaatIni);
            
            if (pesananSaatIni.metodeAmbil != "Batal") {
                int rating;
                cout << "----------------------------------------" << endl;
                cout << "Berikan rating untuk pesanan ini (1-5 bintang): ";
                cin >> rating;
                if (cin.fail() || rating < 1 || rating > 5) {
                    cout << "Rating tidak valid, tidak disimpan." << endl;
                } else {
                    cout << "Terima kasih atas " << rating << " bintangnya!" << endl;
                }
                cin.clear(); // Selalu clear dan ignore setelah cin >> int/char jika ada input berikutnya
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                this_thread::sleep_for(chrono::seconds(2));
            }
        }
        
        if (pesananSaatIni.metodeAmbil == "Batal"){
        }

        cout << "Apakah ada yang ingin Anda pesan lagi? (y/n): ";
        cin >> pesanLagi;

         if (cin.fail()) {
             pesanLagi = 'n';
             cin.clear();
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
         } else {
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
         }

    } while (pesanLagi == 'y' || pesanLagi == 'Y');

    bersihkanLayar();
    cout << "=================================================" << endl;
    cout << "   Terimakasih telah menggunakan Go Food C++ :)   " << endl;
    cout << "=================================================" << endl;

    return 0;
}