// casablancatest.cpp : Defines the entry point for the console application.
//

// Taken from mariusbancila.ro/blog/2013/08/02/cpp-rest-sdk-in-visual-studio-2013

#include "stdafx.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <iostream>
#include <sstream>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

using namespace std;


void print_search_results(json::value const &value)
{
	if(!value.is_null())
	{
		auto response = value[L"responseData"];
		auto results = response[L"results"];
		for(auto const & p : results)
		{
			auto o = p.second;
			auto url = o[L"url"];
			auto title = o[L"titleNoFormatting"];

			wcout << title.as_string() << endl << url.as_string() << endl << endl;
		}
	}
}

void search_and_print(wstring const &searchTerm, int resultsCount)
{
	http_client client(U("https://ajax.googleapis.com/ajax/services/search/web"));

	// build the query parameters
	auto query = uri_builder()
		.append_query(L"q", searchTerm)
		.append_query(L"v", L"1.0")
		.append_query(L"rsz", resultsCount)
		.to_string();

	client
		// send the HTTP GET request asynchronously
		.request(methods::GET, query)
		// continue when the response comes back
		.then([](http_response response) -> pplx::task<json::value>
		{
			// if the status is OK, extract the body into a JSON value
			// only works if the content type is application/json
			if(response.status_code() == status_codes::OK)
			{
				return response.extract_json();
			}

			// return and empty JSON value
			return pplx::task_from_result(json::value());
		})
		// continue when the JSON value is available
		.then([](pplx::task<json::value> previousTask)
		{
			// get the JSON value from the task and display content from it
			try
			{
				json::value const &v = previousTask.get();
				print_search_results(v);
			}
			catch(http_exception const &e)
			{
				wcout << e.what() << endl;
			}
		})
		.wait();
}

void print_thngs(json::value const &value)
{
	if(!value.is_null())
	{
		//wcout << value.as_string() << endl;
		
		auto response = value[L"responseData"];
		auto results = response[L"results"];
		for(auto const & p : results)
		{
			auto o = p.second;
			auto url = o[L"url"];
			auto title = o[L"titleNoFormatting"];

			wcout << title.as_string() << endl << url.as_string() << endl << endl;
		}
	}
}

void get_Thngs()
{
	//http_client client(U("https://api.evrythng.com/thngs"));
	//http_request thngRequest(methods::GET);
	//thngRequest.headers().add(header_names::authorization, L"x3enNKZxGyY8ugFAFwZ6VjcJqIP3vSypHA12H3mxe2BkOE9XrK8LiXXrjKpxY2hqux3MBM2nY51CCw6H");
	//client.request(thngRequest)
	
	//http_client client(U("https://ajax.googleapis.com/ajax/services/search/web"));
	http_client client(U("https://api.evrythng.com/thngs"));

	// build the query parameters
	auto query = uri_builder()		
		.append_query(L"access_token", L"x3enNKZxGyY8ugFAFwZ6VjcJqIP3vSypHA12H3mxe2BkOE9XrK8LiXXrjKpxY2hqux3MBM2nY51CCw6H")
		.to_string();

		client
		// send the HTTP GET request asynchronously
		.request(methods::GET, query)
		// continue when the response comes back
		.then([](http_response response) -> pplx::task<json::value>
		{
			// if the status is OK, extract the body into a JSON value
			// only works if the content type is application/json
			if(response.status_code() == status_codes::OK)
			{
				return response.extract_json();
			}

			// return an empty JSON value
			return pplx::task_from_result(json::value());
		})
		// continue when the JSON value is available
		.then([](pplx::task<json::value> previousTask)
		{
			// get the JSON value from the task and display content from it
			try
			{
				json::value const &thngArray = previousTask.get();
				//print_thngs(v);

				// Loop over each element in the object. 
				for(auto iter = thngArray.cbegin(); iter != thngArray.cend(); ++iter)
				{
					// Make sure to get the value as const reference otherwise you will end up copying 
					// the whole JSON value recursively which can be expensive if it is a nested object. 
					//const json::value &str = iter->first;
					const json::value &thng = iter->second;

					auto thngId = thng[L"id"];
					auto name = thng[L"name"];

					// Perform actions here to process each string and value in the JSON object...
					std::wcout << L"id: " << thngId.as_string() << L", name: " << name.to_string() << endl;
				}
			}
			catch(http_exception const &e)
			{
				wcout << e.what() << endl;
			}
		})
		.wait();
}

// Taken from http://www.codeproject.com/Articles/603810/Using-Casablanca-to-consume-a-REST-API

pplx::task<wstring> create_Thng()
{
	return pplx::create_task([]
	{
		json::value postData;
		postData[L"name"] = json::value::string(L"C++ Thng");
		postData[L"description"] = json::value::string(L"Testing from C++ Rest");

		http_client client(L"https://api.evrythng.com/thngs");

		return client.request(methods::POST, L"", postData.to_string().c_str(), L"application/json");

	}).then([](http_response response) -> pplx::task<json::value>
	{
		if(response.status_code() == status_codes::Created)
		{
			return response.extract_json();			
		}

		// return an empty JSON value
		return pplx::task<json::value>(json::value());
		
				
	})		
	.then([](json::value jsonValue)
	{
				
		const json::value &thng = jsonValue;
		auto id = thng[L"id"].to_string();

		std::wcout << L"Thng Id: " << id;

		return id;
	});
}

int main(int argc, char *args[])
{
	//search_and_print(L"marius bancila", 5);

	//get_Thngs();
	
	create_Thng();

	//get_Thngs();

	std::cin.get();
	
	return 0;
}

