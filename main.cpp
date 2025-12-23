#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace std;

void clearScreen () {
    cout << "\033[H\033[2J";
}

const unsigned KReset   (0);
const unsigned KNoir    (40);
const unsigned KRouge   (41);
const unsigned KVert    (42);
const unsigned KJaune   (43);
const unsigned KBleu    (44);
const unsigned KMagenta (45);
const unsigned KCyan    (46);

void couleur (const unsigned & coul) {
    cout << "\033[" << coul <<"m";
}

typedef vector<unsigned> line;
typedef vector<line>      mat;
typedef pair<unsigned,unsigned> position;
struct maPosition {
    unsigned abs;
    unsigned ord;
};

const unsigned KNbCandies = 6;

void DisplayGrid(const mat & grid) {
    clearScreen();
    couleur(KReset);

    for (const line & row : grid) {
        for (unsigned cel : row) {

            if (cel < 1 || cel > KNbCandies) {
                couleur(KNoir);
                cout << setw(3) << ".";
                couleur(KReset);
            }
            else {
                switch(cel) {
                case 1: couleur(KRouge);   break;
                case 2: couleur(KVert);    break;
                case 3: couleur(KJaune);   break;
                case 4: couleur(KBleu);    break;
                case 5: couleur(KMagenta); break;
                case 6: couleur(KCyan);    break;
                }

                cout << setw(3) << cel;
                couleur(KReset);
            }
        }
        cout << endl;
    }

    cout << endl;
}

void InitGrid(mat & grid, const size_t & taille) {
    grid.resize(taille);
    for (size_t i = 0; i < taille; ++i) {
        grid[i].resize(taille);
        for (size_t j = 0; j < taille; ++j) {
            grid[i][j] = (rand() % KNbCandies) + 1;
        }
    }
}

void makeAMove (mat & grid, const maPosition & pos, const char & direction){
    unsigned i = pos.abs;
    unsigned j = pos.ord;
    unsigned n = grid.size();
    if (i>=n || j>= n) return;
    if ((direction == 'Z' || direction == 'z') && i > 0){
        swap(grid[i][j], grid[i-1][j]);
    }
    else if ((direction =='S' || direction == 's') && i < n-1){
        swap(grid[i][j], grid[i+1][j]);
    }
    else if ((direction =='Q' || direction == 'q') && j > 0){
        swap(grid[i][j], grid[i][j-1]);
    }
    else if ((direction =='D' || direction == 'd') && j < n-1){
        swap(grid[i][j], grid[i][j+1]);
    }
}

int main() {
    srand(time(0));
    maPosition pos;
    char direction;
    mat grid;
    InitGrid(grid, 5);
    DisplayGrid(grid);
    cout << "choisissez la position de la ligne (entre 0 et 4)" << endl;
    cin >> pos.abs;
    cout << "choisissez la position de la ligne (entre 0 et 4)" << endl;
    cin >> pos.ord;
    cout << "choisissez la direction de déplacement (haut = z, bas = s, gauche = q, droite = d" << endl;
    cin >> direction;
    makeAMove(grid, pos , direction);
    DisplayGrid(grid);
    return 0;
}
