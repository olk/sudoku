#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include <boost/logic/tribool.hpp>
#include <boost/logic/tribool_io.hpp>

int Size;

typedef std::vector<boost::tribool> cell_t;

cell_t construct_cell(int size) {
  cell_t result;
  for (int i = 0; i < size * size; i++)
    result.push_back(boost::indeterminate);
  return result;
}

bool mk_true(cell_t& c, int number) {
  if (c[number - 1]) {
    return true;     
  }
  else if (!c[number - 1]) {
    return false;
  }
  else {        
    for (auto it = c.begin(); it != c.end(); it++) {
      * it = false;
    }
    
    c[number - 1] = true;     
  }
  return true;
}

bool mk_false(cell_t& c, int number) {
  if (c[number - 1]) {
    // conflict: was already true
    return false;
  }
  else if (!c[number - 1]) {
    // no conflict: have been assigned already
    return true;     
  }
  else {
    c[number - 1] = false;

    // check whether we have a conflict
    for (int i = 0; i < c.size(); i++) {
      if (c[i]) return true;
      else if (!c[i]) {}
      else return true;
   
    }
    // found no other possibility 
    return false;
     
  }
}

typedef std::vector<std::vector<cell_t> > sudoku_t;

bool mk_true(sudoku_t& s, int x, int y, int k) {
  if (!mk_true(s[x][y], k)) {
    return false;     
  }
      

  int size = sqrt(s.size());
  for (int i = 0; i < size * size; i++) {
    if (i != x) {
      if (!mk_false(s[i][y], k)) {
        return false;
      }         
    }
  }

  for (int i = 0; i < size * size; i++) {
    if (i != y) {
            
      if (!mk_false(s[x][i], k)) {
        return false;             
      }
         
    }
     
  }

  // block
  for (int i = (x / size) * size; i < (x / size) * size + size; i++) {
    for (int j = (y / size) * size; j < (y / size) * size + size; j++) {
      if ((i != x) || (j != y)) {
    if (!mk_false(s[i][j], k)) {
      return false;           
        }
      }  
    }     
  }

    
  return true;
}

void Sudoku_print(const sudoku_t& Sudoku);

sudoku_t construct_sudoku(int size) {
  sudoku_t result;
    
  for (int i = 0; i < size * size; i++) {
    std::vector<cell_t> x;
        
    for (int j = 0; j < size * size; j++) {
      x.push_back(construct_cell(size));
         
    }

    result.push_back(x);
     
  }

  return result;
}

int possibilities(const cell_t& c) {
  int result = 0;
  for (int i = 0; i < c.size(); i++) {
    if (c[i]) {
      return -1;
         
    }
    else if (!c[i]) {
      // dont care   
    }
    else {
      result += 1;         
    }
     
  }

  return result;
}

std::vector<int> ret_possibilitis(const cell_t& c) {
  std::vector<int> result;
  for (int i = 0; i < c.size(); i++) {
    if (c[i]) {      
    }
    else if (!c[i]) {
    }
    else {
      result.push_back(i + 1);         
    }
  }
  return result;
}

int left_number(const cell_t& c) {
    for (int i = 0; i < c.size(); i++) {
        if ( c[i]) {
        } else if ( ! c[i]) {
        } else {
          return i + 1;
        }
    }
    return 10000;
}

const int MAX = 10000;

bool solve( sudoku_t & s) {
    int size = sqrt( s.size() );

    // find promising x, y for propagation or guess
    int x, y, p;

    find_promising :
    x = -1;
    p = MAX;

    for ( int i = 0; i < size * size; i++) {
        for ( int j = 0; j < size * size; j++) {
            int poss_cell = possibilities( s[i][j]);
            // std::cout << "poss_cell = " << poss_cell << "\n";
            // -1 means that it is already assigned a number
            // 0 means it is inconsistent
            // 1 means propagation
            if ( poss_cell == -1) {
            } else if ( poss_cell == 1) {
              p = 1;
              x = i;
              y = j;
              goto propagation;  // forget the rest
            } else if ( poss_cell < p) {
              p = poss_cell;
              x = i;
              y = j;
            }
        }
    }

    // std::cout << "found promising: x = " << x << " y = " << y << " p = " << p << "\n";

    if ( -1 == x) {
        // Solution found
        // std::cout << "solution found\n";
        Sudoku_print(s);
        return true;
    }

    heuristic :
    {
        std::vector<int> rp = ret_possibilitis(s[x][y]);
        for (int i = 0; i < p; i++) {
            // make a copy of s and try out each possibility
            {
                sudoku_t s_cpy = s;
                if ( ! mk_true( s_cpy, x, y, rp[i]) ) {
                    std::cout << "conflict...\n";
                    continue;
                }
                
                // std::cout << "after choosing x = "  << x << " y = " << y << " => " << rp[i] << "\n";
                // Sudoku_print(s_cpy);
                  
                if ( solve( s_cpy) ) {
                    return true;
                }
            }
        }
        return false;
    }

propagation :
    int ln = left_number(s[x][y]);
    // if (ln > s.size()) {
    //   std::cout << "lef"
    // }
    if ( ! mk_true( s, x, y, ln) ) {
        // std::cout << "propagation resulted in conflict\n";
        return false;
    }
    
    // std::cout << "after propagating x = "  << x << " y = " << y << " => " << ln << "\n";
    // Sudoku_print(s);
    
    // no conflict so far, repeat
    goto find_promising;

conflict:
    return false;
}

sudoku_t read(const char* fn) {
    int     i, j, k, l, m;
    FILE*   Stream;
    char*   Buffer;
    char*   Line;
    char*   Tok;
    int Err;
    sudoku_t Sudoku;

    Stream = fopen(fn, "r");

    bool skipped_first = true;

    if (Stream == 0)
    printf("The file %s was not opened.\n", fn);
    else {
    Line = (char*)malloc(sizeof(char) * 500);
    char * Line2 = (char*)malloc(sizeof(char) * 500);

    fscanf(Stream, "experiment: %s\n", Line2);
    fscanf(Stream, "number of tasks: %s\n", Line2);
    fscanf(Stream, "task: %s\n", Line2);
    fscanf(Stream, "puzzle size: %ix%i\n", &Size, &Size);
    printf("experiment: %ix%i\n", Size, Size);

    Sudoku = construct_sudoku(Size);
    std::cout << "size of sudoku = " << sizeof(Sudoku) << "\n";

    i = 1;
    j = 1;
    while (fgets(Line, 500, Stream) != 0) {
      if (Line[0] == '+') {
          continue;
      }
      Tok = strtok(Line, " ");
      while (Tok != 0) {
        if (Tok[0] != '|') {
      k = atoi(Tok);

      if (k != 0) {
        if (!mk_true(Sudoku, i - 1, j - 1, k)) {
          std::cout <<  "fatal\n" << "i = " << i - 1 << " j = " << j - 1 << " k = " << k << "\n";
          exit(1);
                 
            }
          }

      i++;
      if (i % (Size * Size + 1) == 0) {
        i = 1;
        j++;
             
          }
        }

        Tok = strtok(NULL, " ");
             
      }  // end token while
         
    }    // end line while
       
  }

  return Sudoku;
}

int Get_Printable(const cell_t c) {
  for (int i = 0; i < c.size(); i++) {
    if (c[i])
      return i + 1;
     
  }

  return 0;
}

void Sudoku_print(const sudoku_t& Sudoku) {
  int i, j, k, l, m, n;

  if (Size <= 3) {
    n = 1;
     
  }
  else if (Size <= 10) {
    n = 2;
     
  }
  else {
    n = 3;
     
  }

  for (i = 0; i < Size * Size; i++) {
    if (i % Size == 0) {
      for (l = 0; l < Size; l++) {
        printf("+");
        for (m = 0; m < (n + 1) * Size + 1; m++) {
            printf("-");
        }
             
      }
      printf("+\n");
         
    }

    for (j = 0; j < Size * Size; j++) {
      if (j % Size == 0) {
        printf("| ");
             
      }

      // std::cout << "i = " << i << " j = " << j << "\n";
      
      if (n == 1) {
        printf("%i ", Get_Printable(Sudoku[j][i]));
             
      }
      else if (n == 2) {
        if (Get_Printable(Sudoku[j][i]) < 10) {
            printf(" %i ", Get_Printable(Sudoku[j][i]));
        } else {
            printf("%i ", Get_Printable(Sudoku[j][i]));
        }
             
      }
         
    }
    printf("|\n");
  }

  for (l = 0; l < Size; l++) {
    printf("+");
    for (m = 0; m < (n + 1) * Size + 1; m++) {
      printf("-");
         
    }
     
  }
  printf("+\n");
}

int main(int ac, char* av[]) {
    sudoku_t Sudoku  = read( av[1]);
    std::cout << "SOLVE\n";
    if ( solve( Sudoku) ) {
        std::cout << "SAT\n";
    } else {
        std::cout << "UNSAT\n";
    }
    return 0;
}
