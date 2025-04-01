#include <gtest/gtest.h>
// GTest사용해서 문자열관련 기능을 테스트 예제
#include <string>
using std::string;

class StringTest : public ::testing::Test {
  // StringTest는 testing네이스페이스 내의 Test클래스 상속
  // public상속은 자식클래스가 부모클래스의 public , protected멤버에 접근할 수 있게함.
 public:
  string actualString;
  string wrongString;
  string expectString;

  char *actualValTrue;
  char *actualValFalse;
  char *expectVal;

  // Use this method to set up any state that you need for all of your tests
  void SetUp() override {
    actualString = "hello gtest";
    wrongString = "hello world";
    expectString = "hello gtest";

    // 문자열 데이터를 동적 메모리 공간에 복사 (char* 사용)
    actualValTrue = new char[actualString.size() + 1]; 
    //c스타일 문자열은 널 종료문자를 포함해야함.('\0') 그래서 문자길이보다 1 많이 할당.
    //char배열 동적메모리 할당.
    strncpy(actualValTrue, actualString.c_str(), actualString.size() + 1);
    //strncpy(그릇, 내용물, 크기):c스타일 문자열을 다른 문자열로 복사할 때 사용. 
    // actualString의 문자열을 actualValTrue에 복사. 

    actualValFalse = new char[wrongString.size() + 1];
    strncpy(actualValFalse, wrongString.c_str(), wrongString.size() + 1);

    expectVal = new char[expectString.size() + 1];
    strncpy(expectVal, expectString.c_str(), expectString.size() + 1);
  }

  // Use this method to clean up any memory, network etc. after each test
  void TearDown() override { //각 테스트 실행후 자동으로 호출됨.
    delete[] actualValTrue;
    delete[] actualValFalse;
    delete[] expectVal;
    //테스트에서 동적으로 할당한 메모리(new char[])를 해제하여 메모리누수 방지.
  }
};

// Example code we are testing:
namespace myNormalCode {

void reverseInPlace(string &toReverse) { //문자열 뒤집기함수
  // NB! this only works for ASCII 
  for (int i = 0, j = toReverse.size() - 1; i < j; i++, j--) {
    char tmp = toReverse[i];
    toReverse[i] = toReverse[j];
    toReverse[j] = tmp;
  }
}
}


// 테스트케이스. GTest매크로 사용
TEST_F(StringTest, StrEqual) { 
  // GIVEN: two strings that are the same
  // THEN: we expect them to be equal
  EXPECT_STREQ(actualString.c_str(), expectString.c_str()); //두문자열이 같은지 테스트
  //TEST_F(테스트클래스, 테이스이름)
}

/*출력예시
[==========] Running 3 tests from 1 test case.
[----------] Global test environment set-up.
[ RUN      ] StringTest.StrEqual
[       OK ] StringTest.StrEqual (0 ms)
[ RUN      ] StringTest.CStrNotEqual
[       OK ] StringTest.CStrNotEqual (0 ms)
[ RUN      ] StringTest.testReverse
[       OK ] StringTest.testReverse (0 ms)
[----------] 3 tests from 1 test case ran. (0 ms total)
[  PASSED  ] 3 tests.
*/

TEST_F(StringTest, CStrNotEqual) {
  // GIVEN: two char* that are NOT the same

  // THEN: we expect them to be not equal
  EXPECT_STRNE(expectVal, actualValFalse);
}

TEST_F(StringTest, testReverse) {
  // GIVEN: a string, and a manually reversed string as well
  string toReverse = actualString;
  const string expectedReversed = "tsetg olleh";

  // WHEN: we reverse the string
  myNormalCode::reverseInPlace(toReverse);

  // THEN: they should be the same
  EXPECT_STREQ(expectedReversed.c_str(), toReverse.c_str());
}
