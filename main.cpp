#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <set>
#include <map>
#include <stack>
#include <iterator>
#include <time.h>

#define MAX_THREAD_NUM		64
#define MAX_ROUTE_LENGTH	7
#define MIN_ROUTE_LENGTH	2

typedef unsigned int IdNum;

struct User {
	IdNum id;
	std::list<IdNum> outHist;
	int inNum = 0;
	int outNum = 0;
};

struct Node {
	IdNum id;
	Node *front;
	int level;
};

unsigned int userNum = 0;
int thread_num = 0;
int processed_num = 0;
std::map<IdNum, User> user_map;
std::map<int, std::list<std::list<IdNum> > > out_res;
std::set<IdNum> redundent_user;

bool sort_cmp(const std::list<IdNum> &a, const std::list<IdNum> &b)
{
	return *(a.begin()) < *(b.begin());
}

void thread_process(IdNum id_input)
{
	/* Ҫ�������û������û��Ƿ��й���ѭ��ת�ˣ� */
	IdNum user_id = id_input;
//	std::cout << "uid: " << user_id << std::endl;

	std::list<Node> node_buffer;
	Node head;
	head.id = user_id;
	head.front = NULL;		// ͷ�ڵ��parentΪNULL
	head.level = 1;
	node_buffer.push_back(head);

	std::list<Node>::iterator itor = node_buffer.begin();
	while (itor != node_buffer.end()) {
		/* ���itor��Ӧ�ڵ��level�Ѿ�����7�ˣ����Ѿ��ﵽ��󳤶��ˣ���˲���Ҫ�������� */
		if (itor->level > MAX_ROUTE_LENGTH)
			break;

		/* ����ͷ�ڵ㣬�ҵ�ͷ�ڵ��outHist */
		IdNum head_id = itor->id;
		std::list<IdNum> *headOutHist;
		std::map<IdNum, User>::iterator hist_it = user_map.find(head_id);
		if (hist_it != user_map.end())
			headOutHist = &(user_map[head_id].outHist);
		else {
			++itor;
			continue;
		}

		for (std::list<IdNum>::iterator i = (*headOutHist).begin(); i != (*headOutHist).end(); ++i) {
			if (redundent_user.find(*i) != redundent_user.end())
				continue;
			if (*i < user_id)
				continue;
			else if (*i != user_id) {
				/* ����front������ǰ���� */
				Node *ptr = &(*itor);
				while (*i != ptr->id && ptr->front)
					ptr = ptr->front;

				/* �����node_buffer��û���ҵ���صģ������ */
				if (!ptr->front) {
					Node tmp;
					tmp.id = *i;
					tmp.front = &(*itor);
					tmp.level = itor->level + 1;
					node_buffer.push_back(tmp);
				}
			}
			/* ����ҵ���ѭ��ת�˼�¼ */
			else if (*i == user_id && itor->level > MIN_ROUTE_LENGTH && itor->level <= MAX_ROUTE_LENGTH) {
				std::stack<IdNum> id_stk;
				Node *ptr = &(*itor);
				do {
					id_stk.push(ptr->id);
					ptr = ptr->front;
				} while (ptr);
				std::list<IdNum> hist_tmp;
				while (id_stk.size()) {
					hist_tmp.push_back(id_stk.top());
					id_stk.pop();
				}
				out_res[itor->level].push_back(hist_tmp);
				//for (auto &uid : hist_tmp)
				//	std::cout << uid << ' ';
				//std::cout << std::endl;

			}
		}

		++itor;
	}
}

int main()
{
	clock_t start_t, end_t;

	start_t = clock();
	/* ���������ļ� */
	std::ifstream inputFile("/data/test_data.txt");
	std::string line;
	while (std::getline(inputFile, line)) {
		/* ���ļ�����ִ�У����ܸı� */
		std::istringstream ss(line);
		IdNum localId, peerId;
		unsigned int amount;
		char ch;
		ss >> localId >> ch >> peerId >> ch >> amount;
		/*
		 * ��ȡ�ļ���ϣ��ⲿ���ܼ�ֻ��Ҫ0.096�룩
		 * Ҳ����˵������Ĵ���ķ�ʱ��̫��
		 */

		std::map<IdNum, User>::iterator map_itor = user_map.find(localId);
		if (map_itor == user_map.end()) {	// ��֮ǰ�ļ�¼��û���ҵ����û�
			/* �����û���Ϣ */
			User tmp;
			tmp.id = localId;
			tmp.outHist.push_back(peerId);
			tmp.outNum = 1;
			/* �����û���Ϣ */
			user_map.insert(std::make_pair(localId, tmp));
		}
		else {	// ��֮ǰ�ļ�¼���ҵ��˸��û�
			map_itor->second.outNum++;
			map_itor->second.outHist.push_back(peerId);
		}
		// ��¼inHist�Ĳ���
		map_itor = user_map.find(peerId);
		if (map_itor == user_map.end()) {
			User tmp;
			tmp.id = peerId;
			tmp.inNum = 1;
			user_map.insert(std::make_pair(peerId, tmp));
		}
		else {
			map_itor->second.inNum++;
		}

	}
	end_t = clock();

	userNum = user_map.size();
	std::cout << "User Number: " << userNum << std::endl;
	std::cout << "Time to read the file: " << (end_t - start_t) * 1.0 / CLOCKS_PER_SEC << std::endl << std::endl;

	std::list<IdNum> redudent_id;

	std::cout << "kaito is the best" << std::endl;
	start_t = clock();
	/* ��ȥ��Щֻ��ת�����ֻ��ת�����˻���Ϣ */
	for (std::map<IdNum, User>::iterator u = user_map.begin(); u != user_map.end();) {
		if (u->second.inNum == 0 || u->second.outNum == 0) {
			redundent_user.insert(u->first);
			user_map.erase(u++);
		}
		else {
			++u;
		}
	}
	end_t = clock();
	std::cout << "kaito is still the best" << std::endl;

	userNum = user_map.size();
	std::cout << "Effective User Number: " << userNum << std::endl << std::endl;


	//��������
	//IdNum id = 3454;
	//thread_process(6002);
	//thread_process(0);
	//system("pause");
	//return 0;

	start_t = clock();
	//for (std::map<IdNum, User>::iterator i = user_map.begin(); i != user_map.end(); ++i) {
	//	IdNum uid = i->first;
	//	thread_process(uid);
	//}
	for (std::map<IdNum, User>::iterator u = user_map.begin(); u != user_map.end();) {
		IdNum uid = u->first;
		std::list<int> ulist;
		ulist.push_back(uid);
	//	std::cout << uid << std::endl;
		thread_process(uid);
		redundent_user.insert(uid);
		user_map.erase(u++);
	}
	end_t = clock();

	/* ������ */
	std::ofstream resFile("/projects/student/result.txt");
	int total_num = 0;
	for (std::map<int, std::list<std::list<IdNum> > >::iterator lst = out_res.begin(); lst != out_res.end(); ++lst)
		total_num += lst->second.size();
	resFile << total_num << std::endl;
	// i��ʾ����
	for (std::map<int, std::list<std::list<IdNum> > >::iterator i = out_res.begin(); i != out_res.end(); ++i) {
		i->second.sort(sort_cmp);
		// j��ʾ����Ϊi�ĵ�j��·��
		for (std::list<std::list<IdNum> >::iterator j = i->second.begin(); j != i->second.end(); ++j) {
			std::list<IdNum>::iterator it = j->begin();
			std::cout << *it;
			resFile << *it;
			++it;
			for (; it != j->end(); ++it) {
				std::cout << ',' << *it;
				resFile << ',' << *it;
			}
			std::cout << std::endl;
			resFile << std::endl;
		}
	}

	std::cout << (end_t - start_t) * 1.0 / CLOCKS_PER_SEC << std::endl;


#ifdef _MBCS
	system("pause");
#endif
	return 0;


	/* �����̼߳��ϣ���������������ת�˼�¼ */
	//std::map<IdNum, User>::iterator itor_user = user_map.begin();
	//while (processed_num != userNum) {
	//	if (thread_num < MAX_THREAD_NUM) {
	//		IdNum id = itor_user->first;
	//		++itor_user;
	//		thread_num++;

	//		/* �����̣߳�ִ������ */
	//		/*
	//		 * pthread_t thd_id;
	//		 * pthread_create(&thd_id, NULL, thread_process, &id);
	//		 * pthread_join(thd_id, NULL);
	//		 */
	//	}
	//}

	/* wait�߳�ִ���� */

	/* �����̱߳�ǵ�ȫ�ֱ�������������Ϣ */

	std::cout << "process finished!" << std::endl;

#ifdef _MBCS
	system("pause");
#endif

	return 0;
}