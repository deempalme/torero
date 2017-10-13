
#include "headers/console.h"

int main(int argc, char *argv[]){
  Console GUI(argc, argv);
  return GUI.execute(1350, 600, "Torero", false);
}
