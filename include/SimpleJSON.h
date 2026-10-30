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
		const JNode& operator[](const std::string& key) const;
		JNode& operator[](size_t location);
		const JNode& operator[](size_t location) const;
		
		std::string getString(bool last = true, bool prettyPrint = false, std::string extraTabsString = "") const;
		
		bool operator==(const std::string& name) const;
		bool operator!=(const std::string& name) const;
		
		bool operator==(const JNode& node) const;
		bool operator!=(const JNode& node) const;

		JPair& getPair();
		JObject& getObject();
		JArray& getArray();

		const JPair& getPair() const;
		const JObject& getObject() const;
		const JArray& getArray() const;
		
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

		std::string getValue() const;
		std::string escapeInvalidChars() const;
		void setValue(const std::string& n);
		std::string getString(bool last = true, bool prettyPrinting = false, std::string extraTabsString = "") const;
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

		std::string getString(bool last = true, bool prettyPrinting = false, std::string extraTabsString = "") const;
		
		void insert(const JNode& node);
		void insert(JNode&& node);
		void remove(const std::string& name);
		
		JNode& operator[](const std::string& key);
		const JNode& operator[](const std::string& key) const;
		size_t size() const;
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

		std::string getString(bool last = true, bool prettyPrinting = false, std::string extraTabsString = "") const;
		
		JNode& operator[](size_t location);
		const JNode& operator[](size_t location) const;

		JNode& front();
		const JNode& front() const;
		JNode& back();
		const JNode& back() const;

		void push_back(const JNode& node);
		void push_back(JNode&& node);
		void pop_back();
		size_t size() const;
		bool empty() const;
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
		SimpleJSON();
		static SimpleJSON createJSONWithArrayRoot();

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
		SimpleJSON(const File& filename);

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

		/**
		 * @brief Destroy the SimpleJSON object
		 * 
		 */
		~SimpleJSON();

		/**
		 * @brief Get the Root Node for the JSON.
		 *		This may be a object or array node but may never be a pair node.
		 *		All nodes are children of this root node and must be access starting from here.
		 * 
		 * @return JNode& 
		 */
		JNode& getRootNode() const;

		/**
		 * @brief Gets the entire JSON as a string. A wrapper around the root node's getString function that automatically casts
		 *		the root node to its correct type
		 * 
		 * @return std::string 
		 */
		std::string getString(bool prettyPrint = false) const;
		
		/**
		 * @brief Uses the array operator for the root node.
		 *		A simple wrapper to reduce the redundant getRootNode() calls before using the array operator.
		 *			Due to how the class works, this pattern is the only access pattern you can actually use since all other
		 *			aspects of the class are private so reducing the total amount of words to type is useful here.
		 *		Note that due to how the root node for a JSON works, its not known if its an array or object till you check. You may still need to check.
		 * 
		 * @param key 
		 * @return JNode& 
		 */
		JNode& operator[](const std::string& key);
		const JNode& operator[](const std::string& key) const;
		
		/**
		 * @brief Uses the array operator for the root node.
		 *		A simple wrapper to reduce the redundant getRootNode() calls before using the array operator.
		 *			Due to how the class works, this pattern is the only access pattern you can actually use since all other
		 *			aspects of the class are private so reducing the total amount of words to type is useful here.
		 *		Note that due to how the root node for a JSON works, its not known if its an array or object till you check. You may still need to check.
		 * 
		 * @param location
		 * @return JNode& 
		 */
		JNode& operator[](size_t location);
		const JNode& operator[](size_t location) const;

		bool isEmpty() const;

	private:
		void loadJObject(JNode* parent, const std::string& name);
		void loadJArray(JNode* parent, const std::string& name);
		void loadJObjectOrJArray(JNode& node);
		void initLoad();

		
		bool load(Streamable<unsigned char>* data);
		bool load(const File& filename);
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