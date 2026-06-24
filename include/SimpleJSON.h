#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "Streamable.h"
#include <fstream>
#include <vector>
#include "SimpleHashTable.h"
#include <string>

namespace smpl
{
	class DLL_OPTION JNode;
	class DLL_OPTION JPair;
	class DLL_OPTION JArray;
	class DLL_OPTION JObject;

	struct JNodeHash
	{
		using is_transparent = void;
		size_t operator()(const JNode& node) const;
		size_t operator()(const std::string& name) const;
	};
	struct JNodeEquals
	{
		using is_transparent = void;
		bool operator()(const JNode& node, const JNode& node2) const;
		bool operator()(const JNode& node, const std::string& name) const;
		bool operator()(const std::string& name, const JNode& node) const;
	};


	struct JSONInvalidRootElement : public std::exception
	{
		const char* what() const noexcept { return "Root element must be a valid Object or Array"; }
	};
	
	struct JSONInvalidNodeType : public std::exception
	{
		const char* what() const noexcept { return "Node was not one of the three valid node types"; }
	};

	struct JSONParsingError : public std::exception
	{
		const char* what() const noexcept { return "Failed to parse the json data. An Object or Array is not ended properly"; }
	};
	

	class DLL_OPTION JNode
	{
	public:
		static const uint8_t TYPE = 0;
		JNode(uint8_t type, const std::string& name = "");
		JNode(const JNode& other);
		JNode& operator=(const JNode& other);
		JNode(JNode&& other) noexcept;
		JNode& operator=(JNode&& other) noexcept;
		
		~JNode();
		uint8_t getType() const;
		std::string getName() const;
		
		JNode& operator[](const std::string& key);
		JNode& operator[](size_t location);
		
		virtual std::string getString(bool last = true, bool prettyPrint = false, std::string extraTabsString = "");
		
		bool operator==(const std::string& name) const;
		bool operator!=(const std::string& name) const;
		
		bool operator==(const JNode& node) const;
		bool operator!=(const JNode& node) const;

		JPair& getPair();
		JObject& getObject();
		JArray& getArray();
		
	protected:

		union
		{
			//JPair stuff
			std::string value;

			//JObject
			SimpleHashSet<JNode, JNodeHash, JNodeEquals> set; //because it internally uses an array, you can cheat a little

			//JArray
			std::vector<JNode> arr;
		};

	private:

		std::string name;
		uint8_t type = 0;
	};

	class DLL_OPTION JPair : public JNode
	{
	public:
		static const uint8_t TYPE = 1;
		JPair(const std::string& name = "", const std::string& value = "");
		JPair(const JPair& other) = default;
		JPair& operator=(const JPair& other) = default;
		JPair(JPair&& other) = default;
		JPair& operator=(JPair&& other) = default;

		std::string getValue();
		std::string escapeInvalidChars();
		void setValue(const std::string& n);
		virtual std::string getString(bool last = true, bool prettyPrinting = false, std::string extraTabsString = "");
	private:
		void replaceEscapeCodes();
	};

	class DLL_OPTION JObject : public JNode
	{
	public:
		static const uint8_t TYPE = 2;
		JObject(const std::string& name = "");
		JObject(const JObject& other) = default;
		JObject& operator=(const JObject& other) = default;
		JObject(JObject&& other) = default;
		JObject& operator=(JObject&& other) = default;

		virtual std::string getString(bool last = true, bool prettyPrinting = false, std::string extraTabsString = "");
		
		void insert(const JNode& node);
		void insert(JNode&& node);
		void remove(const std::string& name);
		
		JNode& operator[](const std::string& key);
		size_t size();
		bool empty();
		void clear();

		void shrink();

	private:
	};

	class DLL_OPTION JArray : public JNode
	{
	public:
		static const uint8_t TYPE = 3;
		JArray(const std::string& name = "");
		JArray(const JArray& other) = default;
		JArray& operator=(const JArray& other) = default;
		JArray(JArray&& other) = default;
		JArray& operator=(JArray&& other) = default;

		virtual std::string getString(bool last = true, bool prettyPrinting = false, std::string extraTabsString = "");
		
		JNode& operator[](size_t location);
		JNode& front();
		JNode& back();
		void push_back(const JNode& node);
		void push_back(JNode&& node);
		void pop_back();
		size_t size();
		bool empty();
		void clear();

		void shrink();
		// auto begin(){return nodeArr.begin();}
		// auto begin() const {return nodeArr.begin();}
		// auto end(){return nodeArr.end();}
		// auto end() const {return nodeArr.end();}
		
	private:
	};

	class DLL_OPTION SimpleJSON
	{
	public:
		/**
		* @brief Construct a new empty SimpleJSON object.
		*      The root element created is a valid JObject that can be added to.
		*          Note that the root element may be a JArray when loading from other sources.
		* 
		*/
		SimpleJSON(bool arrayRoot);

		/**
		* @brief Construct a new SimpleJSON from an existing data stream.
		*      That data stream is assumed to be a valid JSON as a string
		* 
		* @param data 
		*/
		SimpleJSON(Streamable<unsigned char>* data);

		/**
		* @brief Construct a new SimpleJSON from a file.
		* 
		* @param filename 
		*/
		SimpleJSON(const std::string& filename);

		/**
		* @brief Construct a new SimpleJSON object using an existing complete buffer of data.
		* 
		* @param data 
		* @param size 
		*/
		SimpleJSON(unsigned char* data, size_t size);

		/**
		* @brief Copy Constructor for a SimpleJSON object
		*      Makes a deep copy of the json
		* @param other 
		*/
		SimpleJSON(const SimpleJSON& other);

		/**
		* @brief Copy Assignment for a SimpleJSON object.
		*      Makes a deep copy of the json
		* @param other 
		*/
		SimpleJSON& operator=(const SimpleJSON& other);
		
		/**
		* @brief Move Constructor for a SimpleJSON object.
		*      
		* @param other 
		*/
		SimpleJSON(SimpleJSON&& other) noexcept;

		/**
		* @brief Move Assignment for a SimpleJSON object.
		* 
		* @param other 
		*/
		SimpleJSON& operator=(SimpleJSON&& other) noexcept;

		~SimpleJSON();

		JNode& getRootNode();
	private:
		void loadJObject(JNode* parent, const std::string& name);
		void loadJArray(JNode* parent, const std::string& name);
		void loadJObjectOrJArray(JNode& node);
		void initLoad();

		
		bool load(Streamable<unsigned char>* data);
		bool load(const std::string& filename);
		bool load(unsigned char* data, size_t size);

		bool getNextCharacter(char& c);
		
		Streamable<unsigned char>* datasource = nullptr;
		unsigned char* readBuffer = nullptr;
		size_t readSize = 0;
		size_t readIndex = 0;
		bool continueProcessing = true;

		JNode rootNode = JObject(); //can be an object or array for some stupid reason
	};
}