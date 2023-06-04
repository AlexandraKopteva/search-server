#include "paginator.h"
#include "request_queue.h"
#include "read_input_functions.h"
//#include "remove_duplicates.h"
#include "test_example_functions.h"
#include "process_queries.h"

#include <string>
#include <vector>
#include <execution>

using namespace std;
using namespace std::literals;

void PrintDocument(const Document& document) {
	cout << "{ "s
		<< "document_id = "s << document.id << ", "s
		<< "relevance = "s << document.relevance << ", "s
		<< "rating = "s << document.rating << " }"s << endl;
}


int main() {
	setlocale(LC_ALL, "Russian");
	//TestSearchServer();
	//9 sprint, 7 task
	{
		SearchServer search_server("and with"s);
		int id = 0;
		for (
			const string& text : {
				"white cat and yellow hat"s,
				"curly cat curly tail"s,
				"nasty dog with big eyes"s,
				"nasty pigeon john"s,
			}
			) {
			search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
		}
		cout << "ACTUAL by default:"s << endl;
		// последовательная версия
		for (const Document& document : search_server.FindTopDocuments("curly nasty cat"s)) {
			PrintDocument(document);
		}
		cout << "BANNED:"s << endl;
		// последовательная версия
		for (const Document& document : search_server.FindTopDocuments(execution::seq, "curly nasty cat"s, DocumentStatus::BANNED)) {
			PrintDocument(document);
		}
		cout << "Even ids:"s << endl;
		// параллельная версия
		for (const Document& document : search_server.FindTopDocuments(execution::par, "curly nasty cat"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
			PrintDocument(document);
		}
	}
	//9 sprint, task 9_2
	/*
	{
		SearchServer search_server("and with"s);
		int id = 0;
		for (const string& text : {
				"funny pet and nasty rat"s,
				"funny pet with curly hair"s,
				"funny pet and not very nasty rat"s,
				"pet with rat and rat and rat"s,
				"nasty rat with curly hair"s,
			}
			) {
			search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
		}
		const string query = "curly and funny -not"s;
		{
			const auto[words, status] = search_server.MatchDocument(query, 1);
			cout << words.size() << " words for document 1"s << endl;
			// 1 words for document 1
		}
		{
			const auto[words, status] = search_server.MatchDocument(execution::seq, query, 2);
			cout << words.size() << " words for document 2"s << endl;
			// 2 words for document 2
		}
		{
			const auto[words, status] = search_server.MatchDocument(execution::par, query, 3);
			cout << words.size() << " words for document 3"s << endl;
			// 0 words for document 3
		}
		{
			const auto[words, status] = search_server.MatchDocument(execution::par, query, 4);
			cout << words.size() << " words for document 4"s << endl;
			// 0 words for document 4
		}
		{
			const auto[words, status] = search_server.MatchDocument(execution::par, query, 5);
			cout << words.size() << " words for document 5"s << endl;
			// 1 words for document 5
		}
	}
	//9 sprint, task 9_1
	{
		SearchServer search_server("and with"s);

		int id = 0;
		for (
			const string& text : {
				"funny pet and nasty rat"s,
				"funny pet with curly hair"s,
				"funny pet and not very nasty rat"s,
				"pet with rat and rat and rat"s,
				"nasty rat with curly hair"s,
			}
			) {
			search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
		}

		const string query = "curly and funny"s;

		auto report = [&search_server, &query] {
			cout << search_server.GetDocumentCount() << " documents total, "s
				<< search_server.FindTopDocuments(query).size() << " documents for query ["s << query << "]"s << endl;
		};

		report();
		// однопоточная версия
		search_server.RemoveDocument(5);
		report();
		// однопоточная версия
		search_server.RemoveDocument(std::execution::seq, 1);
		report();
		// многопоточная версия
		search_server.RemoveDocument(std::execution::par, 2);
		report();
	}

	//ProcessQueriesJoined
	{
		SearchServer search_server("and with"s);
		int id = 0;
		for (
			const string& text : {
				"funny pet and nasty rat"s,
				"funny pet with curly hair"s,
				"funny pet and not very nasty rat"s,
				"pet with rat and rat and rat"s,
				"nasty rat with curly hair"s,
			}
			) {
			search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
		}
		const vector<string> queries = {
			"nasty rat -not"s,
			"not very funny nasty pet"s,
			"curly hair"s
		};
		for (const Document& document : ProcessQueriesJoined(search_server, queries)) {
			cout << "Document "s << document.id << " matched with relevance "s << document.relevance << endl;
		}
	}
	//ProcessQueries
	{
		SearchServer search_server("and with"s);
		int id = 0;
		for (
			const string& text : {
				"funny pet and nasty rat"s,
				"funny pet with curly hair"s,
				"funny pet and not very nasty rat"s,
				"pet with rat and rat and rat"s,
				"nasty rat with curly hair"s,
			}
			) {
			search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
		}
		const vector<string> queries = {
			"nasty rat -not"s,
			"not very funny nasty pet"s,
			"curly hair"s
		};
		id = 0;
		for (const auto& documents : ProcessQueries(search_server, queries)) {
			cout << documents.size() << " documents for query ["s << queries[id++] << "]"s << endl;
		}
	}

	//Дедупликатор документов
	{
		SearchServer search_server("and with"s);
		AddDocument(search_server,1, "funny pet and nasty rat"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		AddDocument(search_server,2, "funny pet with curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });
		// дубликат документа 2, будет удалён
		AddDocument(search_server,3, "funny pet with curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });
		// отличие только в стоп-словах, считаем дубликатом
		AddDocument(search_server,4, "funny pet and curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });
		// множество слов такое же, считаем дубликатом документа 1
		AddDocument(search_server,5, "funny funny pet and nasty nasty rat"s, DocumentStatus::ACTUAL, { 1, 2 });
		// добавились новые слова, дубликатом не является
		AddDocument(search_server,6, "funny pet and not very nasty rat"s, DocumentStatus::ACTUAL, { 1, 2 });
		// множество слов такое же, как в id 6, несмотря на другой порядок, считаем дубликатом
		AddDocument(search_server,7, "very nasty rat and not very funny pet"s, DocumentStatus::ACTUAL, { 1, 2 });
		// есть не все слова, не является дубликатом
		AddDocument(search_server,8, "pet with rat and rat and rat"s, DocumentStatus::ACTUAL, { 1, 2 });
		// слова из разных документов, не является дубликатом
		AddDocument(search_server,9, "nasty rat with curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });
		cout << "Before duplicates removed: "s << search_server.GetDocumentCount() << endl;
		RemoveDuplicates(search_server);
		cout << "After duplicates removed: "s << search_server.GetDocumentCount() << endl;
	}
	//проверка времени
	{
		SearchServer search_server("и в на"s);
		search_server.AddDocument(0, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
		search_server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		search_server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		search_server.AddDocument(3, "ухоженный скворец евгений"s, DocumentStatus::BANNED, { 9 });
		//
		FindTopDocuments(search_server, "пушистый ухоженный -кот"s);
		MatchDocument(search_server, "пушистый ухоженный -кот"s, 2);
	}
	//методы поискового сервера
	{
		SearchServer search_server("и в на"s);
		search_server.AddDocument(0, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
		search_server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		search_server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		search_server.AddDocument(3, "ухоженный скворец евгений"s, DocumentStatus::BANNED, { 9 });
		cout << "ACTUAL by default:"s << endl;
		for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный -кот"s)) {
			PrintDocument(document);
		}
		cout << "BANNED:"s << endl;
		for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, DocumentStatus::BANNED)) {
			PrintDocument(document);
		}
		cout << "Even ids:"s << endl;
		for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
			PrintDocument(document);
		}
	}
	//обработка исключений
	{
		try {
			SearchServer search_server("и в на"s);
			search_server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
			//all errors
			//search_server.AddDocument(1, "пушистый пёс и модный ошейник"s, DocumentStatus::ACTUAL, { 1, 2 });
			//search_server.AddDocument(-1, "пушистый пёс и модный ошейник"s, DocumentStatus::ACTUAL, { 1, 2 });
			//search_server.AddDocument(3, "большой пёс скво\x12рец"s, DocumentStatus::ACTUAL, { 1, 3, 2 });

			const auto documents = search_server.FindTopDocuments("кот"s);//--пушистый -
			for (const Document& document : documents) {
				PrintDocument(document);
			}
		}
		catch (const runtime_error& e) {
			cout << e.what() << endl;
		}
		catch (const out_of_range& e) {
			cout << e.what() << endl;
		}
		catch (const invalid_argument& e) {
			cout << e.what() << endl;
		}
		catch (...) {
			cout << "Other exception." << endl;
		}
	}
	//paginator
	{
		SearchServer search_server("and in at"s);
		RequestQueue request_queue(search_server);
		search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
		search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, { 1, 2, 8 });
		search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
		search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, { 1, 1, 1 });
		// 1439 запросов с нулевым результатом
		for (int i = 0; i < 1439; ++i) {
			request_queue.AddFindRequest("empty request"s);
		}
		// все еще 1439 запросов с нулевым результатом
		request_queue.AddFindRequest("curly dog"s);
		// новые сутки, первый запрос удален, 1438 запросов с нулевым результатом
		request_queue.AddFindRequest("big collar"s);
		// первый запрос удален, 1437 запросов с нулевым результатом
		request_queue.AddFindRequest("sparrow"s);
		cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
	}
	*/
	return 0;
}
