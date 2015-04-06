#include <stdio.h>
#include <stdlib.h>
#include <inifile.hh>

int main(int v,char* c[])
{
  IniFile ini;
  char path[256];
  ini.TildeExpansion("fake.ini",path,256);
  printf("%s\n",path);

  char* home;
  home = getenv("HOME");
  printf("%s\n",home);
  return 0;
}
