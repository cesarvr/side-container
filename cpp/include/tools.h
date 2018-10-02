#include <string>
#include <cctype>
#include <vector>

using namespace std;

void count(string ss, vector<char> chrs){
  int cnt =0;
  for(auto i=0; i<ss.length(); i++){
   for(auto chr : chrs)
     if(chr == ss[i])
       cnt++;
  }
  
  cout << "found: " << cnt << " chrs "; 
  for(auto chr : chrs){
    printf(" -> %d ", chr);
  }
  cout << endl; 
}

template <typename WW>
void print_buffer(WW str, int size) {
  cout << "buffer strlen size: " << str.size() << endl;
  cout << "buffer read size: " << size << endl;
  for(int i=0; i<size; i++)
    cout << str[i];
}

template <typename T1, typename T2 >
void analyse(T1 *data1, T2 *data2, int size) {
  printf("analysing... ammount->  %d \n", size);

  for(int i=0; i<size; i++) {
    if(data1[i] != data2[i])
      printf(" data1 = %d !=  data2 = %d \n", data1[i], data2[2]);
  }
  printf("\n");
  printf("\n");
}
