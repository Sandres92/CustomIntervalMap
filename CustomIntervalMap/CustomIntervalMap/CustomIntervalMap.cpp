// CustomIntervalMap.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <algorithm>
#include "gtest/gtest.h"

template <typename K, typename V>
class interval_map {
private:
	V m_valBegin;
	std::map<K, V> m_map;

public:
	template <typename V_forward>
	interval_map(V_forward&& val)
		: m_valBegin(std::forward<V_forward>(val))
	{
	}

	void DebugMap() {
		std::cout << " size " << m_map.size() << "\n";
		std::string res = "";
		for (const auto& [key, val] : m_map)
		{
			std::cout << key << " , " << val << "\n";
		}
		std::cout << " =======\n";
	}

	std::string GetString() {
		std::string res = "";
		for (const auto& [key, val] : m_map)
		{
			res += std::to_string(key);
			res += ": ";
			res += val;
			res += ", ";
		}
		return res;

	}

	template<typename V_forward>
	void assign(K const& keyBegin, K const& keyEnd, V_forward&& val)
		requires (std::is_same<std::remove_cvref_t<V_forward>, V>::value)
	{
		if (!(keyBegin < keyEnd))
		{
			return;
		}

		auto itLeft = m_map.upper_bound(keyBegin);
		//auto valLeft = m_valBegin;
		auto valDefault = m_valBegin;

		bool isValBegin = (val == m_valBegin);

		if (itLeft == m_map.end()) {

			if (!isValBegin) {
				//m_map[keyBegin] = std::forward<V_forward>(val);
				//m_map[keyEnd] = m_valBegin;

				m_map.insert_or_assign(keyBegin, std::forward<V_forward>(val));
				m_map.insert_or_assign(keyEnd, m_valBegin);
			}

			return;
		}

		if (itLeft == m_map.begin()) {

			if ((!(itLeft->second == val) || keyBegin < itLeft->first) && !isValBegin) {
				m_map.emplace_hint(itLeft, keyBegin, std::forward<V_forward>(val));
			}
		}
		else if (itLeft != m_map.begin() && itLeft != m_map.end()) {

			auto prev = std::prev(itLeft);
			bool isKeysEq = !(prev->first < keyBegin) && !(keyBegin < prev->first);
			bool isValEq = prev->second == val;

			if (isKeysEq && !isValEq && !isValBegin) {
				//m_map[keyBegin] = val;
				m_map.insert_or_assign(keyBegin, std::forward<V_forward>(val));
				//m_map[keyBegin] = std::forward<V_forward>(val);
			}
			else if (isKeysEq && isValBegin) {
				itLeft = prev;
				valDefault = prev->second;
			}
			else if (prev->first < keyBegin) {
				//itLeft = prev;
				valDefault = prev->second;
				if (!isValEq) {
					m_map.emplace_hint(itLeft, keyBegin, std::forward<V_forward>(val));
				}
			}
			else if (prev->first < keyBegin && isValEq) {
				//m_map[keyBegin] = val;
				valDefault = std::forward<V_forward>(val);
				m_map.emplace_hint(itLeft, keyBegin, valDefault);
			}
		}

		auto itRight = itLeft;
		//auto valRight = valLeft; 

		for (; itRight != m_map.end() && !(keyEnd < itRight->first); ++itRight) {
			valDefault = itRight->second;
		}

		if (itLeft != itRight) {
			m_map.erase(itLeft, itRight);
		}

		if (itRight == m_map.begin()) {

			if (m_map.begin() != m_map.end()) {
				m_map.emplace_hint(itRight, keyEnd, valDefault);
			}
		}
		else if (itRight == m_map.end()) {
			m_map.emplace_hint(itRight, keyEnd, valDefault);
		}
		else {
			auto itPrev = std::prev(itRight);
			if (itPrev->second != valDefault) {
				m_map.emplace_hint(itRight, keyEnd, valDefault);
			}
		}
	}

	V const& operator[](K const& key) const
	{
		auto it = m_map.upper_bound(key);
		if (it == m_map.begin()) {
			return m_valBegin;
		}
		else {
			return (--it)->second;
		}
	}
};

TEST(MyTestSuite, BasicTest1) {
	interval_map<int, char> m('A');

	m.assign(1, 3, 'B');

	EXPECT_EQ(m.GetString(), "1: B, 3: A, ");
}

TEST(MyTestSuite, BasicTest2) {
	interval_map<int, char> m('A');

	m.assign(2, 5, 'A');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "");
}

TEST(MyTestSuite, BasicTest3) {
	interval_map<int, char> m('A');

	m.assign(1, 3, 'B');
	m.assign(5, 7, 'C');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "1: B, 3: A, 5: C, 7: A, ");
}

TEST(MyTestSuite, BasicTest4) {
	interval_map<int, char> m('A');

	m.assign(4, 4, 'B');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "");
}

TEST(MyTestSuite, BasicTest5) {
	interval_map<int, char> m('A');

	m.assign(1, 5, 'B');
	m.assign(2, 4, 'B');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "1: B, 5: A, ");
}

TEST(MyTestSuite, BasicTest6) {
	interval_map<int, char> m('A');

	m.assign(1, 3, 'B');
	m.assign(2, 4, 'C');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "1: B, 2: C, 4: A, ");
}

TEST(MyTestSuite, BasicTest7) {
	interval_map<int, char> m('A');

	m.assign(1, 3, 'B');
	m.assign(3, 5, 'A');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "1: B, 3: A, ");
}

TEST(MyTestSuite, BasicTest8) {
	interval_map<int, char> m('A');

	m.assign(-1000, 1000, 'B');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "-1000: B, 1000: A, ");
}

TEST(MyTestSuite, BasicTest9) {
	interval_map<int, char> m('A');

	m.assign(0, 10, 'B');
	m.assign(0, 10, 'A');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "");
}

TEST(MyTestSuite, BasicTest10) {
	interval_map<int, char> m('A');

	m.assign(0, 2, 'B');
	//m.assign(5, 7, 'C');
	m.assign(2, 4, 'A');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "0: B, 2: A, ");
}

TEST(MyTestSuite, BasicTest11) {
	interval_map<int, char> m('A');

	//падает
	m.assign(10, 20, 'B');
	m.assign(12, 15, 'C');
	m.assign(9, 21, 'D');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "9: D, 21: A, ");
}

TEST(MyTestSuite, BasicTest12) {
	interval_map<int, char> m('A');

	m.assign(1, 2, 'B');
	m.assign(2, 4, 'C');
	m.assign(4, 6, 'B');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "1: B, 2: C, 4: B, 6: A, ");
}

TEST(MyTestSuite, BasicTest13) {
	interval_map<int, char> m('A');

	m.assign(1, 2, 'B');
	m.assign(-4, -2, 'C');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "-4: C, -2: A, 1: B, 2: A, ");
}

TEST(MyTestSuite, BasicTest14) {
	interval_map<int, char> m('A');

	m.assign(1, 2, 'B');
	m.assign(-4, 1, 'C');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "-4: C, 1: B, 2: A, ");
}

TEST(MyTestSuite, BasicTest15) {
	interval_map<int, char> m('A');

	m.assign(-4, 2, 'B');
	m.assign(-2, 0, 'A');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "-4: B, -2: A, 0: B, 2: A, ");
}

TEST(MyTestSuite, BasicTest16) {
	interval_map<int, char> m('A');

	m.assign(-10, -5, 'B');
	m.assign(-2, 2, 'C');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "-10: B, -5: A, -2: C, 2: A, ");
}

TEST(MyTestSuite, BasicTest17) {
	interval_map<int, char> m('A');

	m.assign(5, 10, 'B');
	m.assign(0, 5, 'C');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "0: C, 5: B, 10: A, ");
}

TEST(MyTestSuite, BasicTest18) {
	interval_map<int, char> m('A');

	m.assign(5, 10, 'B');
	m.assign(0, 5, 'C');
	m.assign(7, 9, 'D');

	//m.assign(1, 3, 'B');
	EXPECT_EQ(m.GetString(), "0: C, 5: B, 7: D, 9: B, 10: A, ");
}

TEST(MyTestSuite, BasicTest19) {
	interval_map<int, char> m('A');

	m.assign(9, 10, 'D');
	m.assign(5, 7, 'D');
	m.assign(0, 5, 'D');

	EXPECT_EQ(m.GetString(), "0: D, 7: A, 9: D, 10: A, ");
}

TEST(MyTestSuite, BasicTest20) {
	interval_map<int, char> m('A');

	m.assign(20, 30, 'X');
	m.assign(10, 20, 'X');
	m.assign(0, 10, 'A');

	EXPECT_EQ(m.GetString(), "10: X, 30: A, ");
}

TEST(MyTestSuite, BasicTest21) {
	interval_map<int, char> m('A');

	m.assign(10, 15, 'A');
	m.assign(5, 10, 'X');
	m.assign(0, 5, 'X');

	EXPECT_EQ(m.GetString(), "0: X, 10: A, ");
}

TEST(MyTestSuite, BasicTest22) {
	interval_map<int, char> m('A');

	m.assign(0, 10, 'Z');
	m.assign(5, 10, 'Y');
	m.assign(3, 7, 'X');

	EXPECT_EQ(m.GetString(), "0: Z, 3: X, 7: Y, 10: A, ");
}

TEST(MyTestSuite, BasicTest23) {
	interval_map<int, char> m('A');

	m.assign(100, 200, 'Q');
	m.assign(90, 100, 'Q');
	m.assign(50, 90, 'Q');

	EXPECT_EQ(m.GetString(), "50: Q, 200: A, ");
}

TEST(MyTestSuite, BasicTest24) {
	interval_map<int, char> m('A');

	m.assign(0, 10, 'B');
	m.assign(10, 20, 'C');
	m.assign(20, 30, 'D');

	EXPECT_EQ(m.GetString(), "0: B, 10: C, 20: D, 30: A, ");
}

TEST(MyTestSuite, BasicTest25) {
	interval_map<int, char> m('A');

	m.assign(0, 100, 'B');
	m.assign(40, 60, 'C');

	EXPECT_EQ(m.GetString(), "0: B, 40: C, 60: B, 100: A, ");
}

TEST(MyTestSuite, BasicTest27) {
	interval_map<int, char> m('A');

	m.assign(0, 50, 'B');
	m.assign(25, 75, 'C');
	m.assign(60, 90, 'D');

	EXPECT_EQ(m.GetString(), "0: B, 25: C, 60: D, 90: A, ");
}

TEST(MyTestSuite, BasicTest28) {
	interval_map<int, char> m('A');

	m.assign(0, 10, 'B');
	m.assign(10, 20, 'C');
	m.assign(20, 30, 'D');

	m.assign(10, 20, 'E');

	EXPECT_EQ(m.GetString(), "0: B, 10: E, 20: D, 30: A, ");
}

TEST(MyTestSuite, BasicTest29) {
	interval_map<int, char> m('A');

	m.assign(90, 100, 'J');
	m.assign(80, 90, 'I');
	m.assign(70, 80, 'H');
	m.assign(60, 70, 'G');
	m.assign(50, 60, 'F');

	EXPECT_EQ(m.GetString(), "50: F, 60: G, 70: H, 80: I, 90: J, 100: A, ");
}

TEST(MyTestSuite, BasicTest30) {
	interval_map<int, char> m('A');

	m.assign(-1000, -900, 'X');
	m.assign(1000, 1100, 'Y');
	m.assign(-950, 1050, 'Z');

	EXPECT_EQ(m.GetString(), "-1000: X, -950: Z, 1050: Y, 1100: A, ");
}

TEST(MyTestSuite, BasicTest31) {
	interval_map<int, char> m('A');

	m.assign(0, 100, 'A');
	m.assign(20, 80, 'B');
	m.assign(40, 60, 'C');
	m.assign(50, 55, 'A');
	m.assign(45, 65, 'D');

	EXPECT_EQ(m.GetString(), "20: B, 40: C, 45: D, 65: B, 80: A, ");
}

void TestMap() {

	std::map<int, char> m_map;
	//m_map.insert({ -1, 'C' });
	m_map.insert({ 1, 'B' });
	m_map.insert({ 3, 'A' });

	auto it = m_map.begin();
	it++;

	m_map.erase(it, m_map.end());
	std::cout << "________________  " << m_map.size() << "\n";

	return;
	auto itLeft = m_map.lower_bound(1);
	if (itLeft == m_map.begin()) {
		std::cout << "Begin L \n";
	}
	else if (itLeft == m_map.end()) {
		std::cout << "End L\n";
	}
	else {
		std::cout << " L " << itLeft->first << " , " << itLeft->second << "\n";
	}

	auto itRight = m_map.lower_bound(5);
	if (itRight == m_map.begin()) {
		std::cout << "Begin R \n";
	}
	else if (itRight == m_map.end()) {
		std::cout << "End R \n";
	}
	else {
		std::cout << " R " << itRight->first << " , " << itRight->second << "\n";
	}
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
