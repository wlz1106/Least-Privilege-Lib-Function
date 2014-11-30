#include<stdio.h>
#include<string.h>


union Cipher {
  struct Message {
    unsigned int sender;
    unsigned int receiver;
    char content[119];
  } input;
 struct Message1 {
 unsigned int sender;
 unsigned int receiver;
 char content[119];
 }output;
 
 
};
/* Union + Cesar Cipher function
 * ------------------------------
 *
 * encrypt function takes as input a tweet and an offset, and produces a
 * encrypted message. The input to the function is obtained from a tweet. Each
 * tweet is a message that is in the following format:
 *
 * SSSSSSSSSS RRRRRRRRRR MMMMM(...upto 118 characters)
 * [---10---] [---10---] [-----------118-------------]
 * [---------------------140-------------------------]
 *
 * It starts with the sender phone number, followed by a space, followed by the
 * receiver phone number, followed by a space, and upto 118 characters of
 * message. The maximum length of such a message will be 140 characters. The
 * phone numbers start with either 1, 2 or 3 (restricted so that the number
 * will fit in an unsigned int).
 *
 * Encryption is achieved through the union Cipher described above. The idea is
 * to fill in the input structure, and read from the output structure (to be
 * defined by you). This will produce a HEX equivalent of the input message.
 * Finally, the message is shifted (Cesar cipher) by (offset % 16) in the
 * Hex space.
 *
 * The function returns the result in the string encryptedTweet, which contains
 * the encrypted message.
 */

void encrypt (char* tweet, int offset, char* encryptedTweet) {
  union Cipher c;
  offset=offset%16;
  c.input.sender=0;
  c.input.receiver=0;
	
  int i;
  for(i=0;i<10;i++){
  	c.input.sender=c.input.sender*10+tweet[i]-'0';
  }
  int j;
  for(j=11;j<21;j++){
  	c.input.receiver=c.input.receiver*10+tweet[j]-'0';
  }
  int k;
  for(k=22;k<140;k++){
  	
  	c.input.content[k-22]=tweet[k];
	if(tweet[k]=='\0')break;
  }
  c.input.content[119]='\0';

  char * cp= (char *)(&c);
 k=0;
 int m;
 int size=sizeof(c);
 for(m=1;m<=size;m++){
	
  	char n=0;
	n=(unsigned char)(*cp&0b11110000)>>4;
	
	n+=offset;
	n=n&0b00001111;
	
	if(n>9)encryptedTweet[k++]='A'+n-10; 
	else encryptedTweet[k++]='0'+n;
	
	n=*cp&0b00001111;
	
	n+=offset;
	n=n&0b00001111;
	
	if(n>9)encryptedTweet[k++]='A'+n-10;
	else encryptedTweet[k++]='0'+n;
	
	cp++;
  }
}


/* Decipher function
 * -----------------
 *
 * Decrypt function is given the encrypted tweet and the offset (used for
 * negative shift in the Cesar cipher). The function returns the result in the
 * tweet string.
 */

void decrypt (char* encryptedTweet, int offset, char* tweet) {
  union Cipher c;
  offset=offset%16;
  c.output.sender=0;
  c.output.receiver=0;
  char * cp= (char *)(&c.output);
  int i=0;
 int size=2*sizeof(c);
  while(i<size){
	char d=0;
  	if(encryptedTweet[i]<'A')encryptedTweet[i]-='0';
	else encryptedTweet[i]-=('A'-10);
        if(encryptedTweet[i]<offset)encryptedTweet[i]+=16;
	encryptedTweet[i]-=offset;
	d = encryptedTweet[i++]<<4;
	if(encryptedTweet[i]<'A')encryptedTweet[i]-='0';
	else encryptedTweet[i]-=('A'-10);
        if(encryptedTweet[i]<offset)encryptedTweet[i]+=16;
	encryptedTweet[i]-=offset;
	d +=encryptedTweet[i++];
	*cp=d;
	cp++;	
  }
	int temp=c.output.sender;
  for(i=9;i>=0;i--){
  	tweet[i]=temp%10+'0';
	temp /= 10;
  }
  tweet[10]=' ';
  temp=c.output.receiver;
  for(i=20;i>=11;i--){
  	tweet[i]=temp%10+'0';
	temp /= 10;
  }
  tweet[21]=' ';
  int k;
  for(k=22;k<141;k++){
  	tweet[k]=c.output.content[k-22];
  }
	printf("%s\n",tweet);
}
int main(){
	
	char c[141];
	int i=0;
	
	do{
		char a=getchar();
		c[i]=a;
	}while(c[i]!=EOF&&c[i++]!='\0');
	char e[2*sizeof(union Cipher)];
	
	printf("%s end\n",c);
	encrypt(c,5,e);
	//printf("===============begin==================\n");
	//printf("%s\n",e);
	//printf("%d\n",(int)strlen(e));
	
         decrypt(e,5,c);
	 //printf("=================end================\n");
	//printf("%s",c);	
}



