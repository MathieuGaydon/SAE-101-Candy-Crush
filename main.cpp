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
const unsigned KImpossible = 0;

void displayGrid(const mat & grid) {
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

void initGrid(mat & grid, const size_t & taille) {
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

bool atLeastThreeInAColumn (const mat &grid, maPosition & pos, unsigned & howMany){
    unsigned n = grid.size();

    for (unsigned j = 0; j < n; ++j) {
        for (unsigned i = 0; i < n ; ++i){
            unsigned value = grid[i][j];
            if (value != KImpossible){
                unsigned count = 1;
                unsigned k = i + 1;
                while (k < n && grid[k][j] == value){
                    ++count;
                    ++k;
                }
                if (count >= 3){
                    pos.abs = i;
                    pos.ord = j;
                    howMany = count;
                    return true;
                }
                i = k - 1;
            }
        }
    }
    return false;
}

bool atLeastThreeInARow (const mat &grid, maPosition & pos, unsigned & howMany){
    unsigned n = grid.size();

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < n ; ++j){
            unsigned value = grid[i][j];
            if (value != KImpossible){
                unsigned count = 1;
                unsigned k = j + 1;
                while (k < n && grid[i][k] == value){
                    ++count;
                    ++k;
                }
                if (count >= 3){
                    pos.abs = i;
                    pos.ord = j;
                    howMany = count;
                    return true;
                }
                j = k - 1;
            }
        }
    }
    return false;
}

void removalInColumn (mat &grid, const maPosition &pos, unsigned howMany) {
    unsigned col = pos.ord;

    for (unsigned i = pos.abs; i < pos.abs + howMany; ++i){
        grid[i][col] = KImpossible;
    }

    for (unsigned i = pos.abs + howMany; i < grid.size(); ++i) {
        unsigned k = i;
        while (k > 0 && grid[k - 1][col] == KImpossible) {
            swap(grid[k][col], grid[k - 1][col]);
            --k;
        }
    }
}

void removalInRow(mat & grid, const maPosition & pos, unsigned howMany){
    for (unsigned j = 0; j < howMany; ++j) {
        maPosition p;
        p.abs = pos.abs;
        p.ord = pos.ord + j;

        removalInColumn(grid, p, 1);
    }
}

int main() {
    srand(time(0));

    const unsigned taille = 5;
    const unsigned maxCoups = 10;

    mat grid;
    maPosition pos;
    char direction;
    unsigned score = 0;

    initGrid(grid, taille);

    for (unsigned coup = 1; coup <= maxCoups; ++coup) {
        displayGrid(grid);

        cout << "Coup " << coup << "/" << maxCoups << endl;
        cout << "Score :" << score << endl << endl;

        cout << "Ligne (0-" << taille-1<<") : ";
        cin >> pos.abs;

        cout << "Colonne (0-" << taille-1<<") : ";
        cin >> pos.ord;

        cout << "Deplacement (z=haut, s=bas, q=gauche, d=droite) : ";
        cin >> direction;

        makeAMove(grid, pos, direction);

        displayGrid(grid);

        unsigned howMany;

        if(atLeastThreeInAColumn(grid, pos, howMany)){
            cout << "Alignement vertical détecté !" << endl;
            removalInColumn(grid, pos, howMany);
            score += howMany;
        }
        else if(atLeastThreeInARow(grid, pos, howMany)){
            cout << "Alignement horizontal détecté !" << endl;
            removalInRow(grid, pos, howMany);
            score += howMany;
        }
        else{
            cout << "Aucun alignement." << endl;
        }

        displayGrid(grid);
        cout << "Appuyez sur entree pour continuer";
        cin.ignore();
        cin.get();
    }
    displayGrid(grid);

    cout << "Partie terminée !" << endl;
    cout << "Score Final : " << score << endl;

    return 0;
}
