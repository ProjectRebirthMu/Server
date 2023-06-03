#pragma once

#include <windows.h> 
#include <string> 
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h> 


using namespace std;

// first, some types:
  typedef unsigned       int uint4; // assumes integer is 4 words long
  typedef unsigned short int uint2; // assumes short integer is 2 words long
  typedef unsigned      char uint1; // assumes char is 1 word long

class MD5 
{
public:

//---------------------------------------------
//	MD5 �߰��� �޼����
bool MD5_EncodeKeyVal	(					// �Էµ� ���ڿ��� Ű�ε��� (0~255)�� ����Ͽ� 128 bits (16 bytes) �� MD5 Ű�� ����
	  char * lpszInputStr,					// �Է� ���ڿ�
	  char * lpszOutputKeyVal,				// ��� ����
	  int iKeyIndex							// Ű�ε��� (0~255)
	  );

bool MD5_EncodeString	(					// �Էµ� ���ڿ��� Ű�ε��� (0~255)�� ����Ͽ� 128 x 2 bits (32 bytes) �� MD5 ���ڿ� Ű�� ����
	  char * lpszInputStr,					// �Է� ���ڿ�
	  char * lpszOutputStr,					// ��� ����
	  int iKeyIndex							// Ű�ε��� (0~255)
	  );

bool MD5_CheckValue	(						// ���ڿ��� MD5 Ű���� Ű�ε��� (0~255) �� �Է¹޾� Ű���� ������ (true : ���� / false : Ʋ��)
											// P.S.> Ű���� ���ڿ�(256 bits)�� �ƴ� 128 bits MD5 Ű���� �Ѵ�.
	  char * lpszInputStr,					// �Է� ���ڿ�
	  char * szKeyVal,						// �Է� MD5 Ű��
	  int iKeyIndex							// Ű�ε��� (0~255)
	  );


//---------------------------------------------
//	MD5 ���� �޼����

// methods for controlled operation:
  MD5              ();  // simple initializer
  void  update     (uint1 *input, uint4 input_length);
  void  update     (std::istream& stream);
  void  update     (FILE *file);
  void  update     (std::ifstream& stream);
  void  finalize   ();

// constructors for special circumstances.  All these constructors finalize
// the MD5 context.
  MD5              (unsigned char *string); // digest string, finalize
  MD5              (std::istream& stream);       // digest stream, finalize
  MD5              (FILE *file);            // digest file, close, finalize
  MD5              (std::ifstream& stream);      // digest stream, close, finalize

// methods to acquire finalized result
  unsigned char    *raw_digest ();  // digest as a 16-byte binary array
  char *            hex_digest ();  // digest as a 33-byte ascii-hex string
  friend std::ostream&   operator<< (std::ostream&, MD5 context);



private:

//---------------------------------------------
//	MD5 �߰��� �޼����
  void setmagicnum (int keyindex);


//---------------------------------------------
//	MD5 �߰��� ��� ������
	unsigned char	m_cRaw_digest[16];  
	char			m_cHex_digest[33];  



//---------------------------------------------
//	MD5 ���� ��� ������



// next, the private data:
  uint4 state[4];
  uint4 count[2];     // number of *bits*, mod 2^64
  uint1 buffer[64];   // input buffer
  uint1 digest[16];
  uint1 finalized;

// last, the private methods, mostly static:
  void init             ();               // called by all constructors
  void transform        (uint1 *buffer);  // does the real update work.  Note 
                                          // that length is implied to be 64.

  static void encode    (uint1 *dest, uint4 *src, uint4 length);
  static void decode    (uint4 *dest, uint1 *src, uint4 length);
  static void memcpy    (uint1 *dest, uint1 *src, uint4 length);
  static void memset    (uint1 *start, uint1 val, uint4 length);

  static inline uint4  rotate_left (uint4 x, uint4 n);
  static inline uint4  F           (uint4 x, uint4 y, uint4 z);
  static inline uint4  G           (uint4 x, uint4 y, uint4 z);
  static inline uint4  H           (uint4 x, uint4 y, uint4 z);
  static inline uint4  I           (uint4 x, uint4 y, uint4 z);
  static inline void   FF  (uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
			    uint4 s, uint4 ac);
  static inline void   GG  (uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
			    uint4 s, uint4 ac);
  static inline void   HH  (uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
			    uint4 s, uint4 ac);
  static inline void   II  (uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
			    uint4 s, uint4 ac);

};


// Constants for MD5Transform routine.
// Although we could use C++ style constants, defines are actually better,
// since they let us easily evade scope clashes.

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

