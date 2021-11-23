// tested source file, dont compile it
void SomeFunc(int param, int param2){
	int _local_var = param + param2;
}

int main(){
	int decNum = 12;
	long hexNum = 0x1b;
	short octNum = 017;
	bool boolNum = decNum == hexNum;
	decNum = hexNum * (octNum - 3) % 5;a
	hexNum++;
	++hexNum;
	--octNum;
	octNum--;
	for(int i = 0; i < 10; i++){
		hexNum = hexNum / 2;
	}
	boolNum = decNum > octNum != octNum < hexNum;
	boolNum = octNum >= decNum != decNum <= hexNum;
	// errors
	decNum = 12de_;
	hexNum = 0x;
	hexNum = 0x23fg2;
	octNum = 019;
	octNum = !02a;
}
// some comment