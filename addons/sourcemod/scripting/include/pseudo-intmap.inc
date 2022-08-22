#if defined _intmap_included
 #endinput
#endif
#define _intmap_included

#if !defined _pseudo_intmap
#define _pseudo_intmap
#endif

typeset IntMapLoopCallback
{
	function Action (IntMap map, int index, int key, any data);
	function Action (IntMap map, int index, int key);
};

methodmap IntMapInternal __nullable__
{
	public IntMapInternal() {}

	public void __PackIntToStr(const int i, char s[6])
	{
		s[0] = ((i >> 28) & 0x7F) | 0x80;
		s[1] = ((i >> 21) & 0x7F) | 0x80;
		s[2] = ((i >> 14) & 0x7F) | 0x80;
		s[3] = ((i >> 7) & 0x7F) | 0x80;
		s[4] = ((i) & 0x7F) | 0x80;
		s[5] = 0;
	}

	public int __ExtractIntFromStr(const char s[6])
	{
		int i = 0;
		i |= ((s[0] & 0xF) << 28);
		i |= ((s[1] & 0x7F) << 21);
		i |= ((s[2] & 0x7F) << 14);
		i |= ((s[3] & 0x7F) << 7);
		i |= ((s[4]) & 0x7F);
		return i;
	}
}

methodmap IntMapSnapshot < Handle
{
	// Returns the number of keys in the map snapshot.
	property int Length
	{
		public get() {
			return view_as<StringMapSnapshot>(this).Length;
		}
	}

	// Retrieves the key of a given index in a map snapshot.
	//
	// @param index      Key index (starting from 0).
	// @return           The key.
	// @error            Index out of range.
	public int GetKey(int index)
	{
		if (index < 0 || index >= this.Length) {
			ThrowError("Index out of range");
		}

		static char _key[6];
		view_as<StringMapSnapshot>(this).GetKey(index, _key, 6);
		return view_as<IntMapInternal>(0).__ExtractIntFromStr(_key);
	}
}

methodmap IntMap < Handle
{
	// The IntMap must be freed via delete or CloseHandle().
	public IntMap()
	{
		return view_as<IntMap>(new StringMap());
	}

	// Sets a value in a Map, either inserting a new entry or replacing an old one.
	//
	// @param key        The key.
	// @param value      Value to store at this key.
	// @param replace    If false, operation will fail if the key is already set.
	// @return           True on success, false on failure.
	public bool SetValue(const int key, any value, bool replace = true)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		return view_as<StringMap>(this).SetValue(_key, value, replace);
	}

	// Sets an array value in a Map, either inserting a new entry or replacing an old one.
	//
	// @param key        The key.
	// @param array      Array to store.
	// @param num_items  Number of items in the array.
	// @param replace    If false, operation will fail if the key is already set.
	// @return           True on success, false on failure.
	public bool SetArray(const int key, const any[] array, int num_items, bool replace = true)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		return view_as<StringMap>(this).SetArray(_key, array, num_items, replace);
	}

	// Sets a string value in a Map, either inserting a new entry or replacing an old one.
	//
	// @param key        The key.
	// @param value      String to store.
	// @param replace    If false, operation will fail if the key is already set.
	// @return           True on success, false on failure.
	public bool SetString(const int key, const char[] value, bool replace = true)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		return view_as<StringMap>(this).SetString(_key, value, replace);
	}

	// Retrieves a value in a Map.
	//
	// @param key        The key.
	// @param value      Variable to store value.
	// @return           True on success.  False if the key is not set, or the key is set
	//                   as an array or string (not a value).
	public bool GetValueEx(const int key, any& value)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		return view_as<StringMap>(this).GetValue(_key, value);
	}

	public any GetValue(const int key)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		static int value;
		if (!view_as<StringMap>(this).GetValue(_key, value)) {
			return 0;
		}
		return value;
	}

	// Retrieves an array in a Map.
	//
	// @param key        The key.
	// @param array      Buffer to store array.
	// @param max_size   Maximum size of array buffer.
	// @param size       Optional parameter to store the number of elements written to the buffer.
	// @return           True on success.  False if the key is not set, or the key is set
	//                   as a value or string (not an array).
	public bool GetArray(const int key, any[] array, int max_size, int& size = 0)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		return view_as<StringMap>(this).GetArray(_key, array, max_size, size);
	}

	public any GetArrayCell(const int key, int index)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		int[] buffer = new int[index + 1];
		int size = 0;
		if (!view_as<StringMap>(this).GetArray(_key, buffer, index + 1, size)) {
			return 0;
		}
		if (size <= index) {
			LogError("Invalid index %i max: %i", index, size);
			return 0;
		}
		return buffer[index];
	}

	// Retrieves a string in a Map.
	//
	// @param key        The key.
	// @param value      Buffer to store value.
	// @param max_size   Maximum size of string buffer.
	// @param size       Optional parameter to store the number of bytes written to the buffer.
	// @return           True on success.  False if the key is not set, or the key is set
	//                   as a value or array (not a string).
	public bool GetString(const int key, char[] value, int max_size, int& size = 0)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		return view_as<StringMap>(this).GetString(_key, value, max_size, size);
	}

	// Removes a key entry from a Map.
	//
	// @param key        The key.
	// @return           True on success, false if the value was never set.
	public bool Remove(const int key)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		return view_as<StringMap>(this).Remove(_key);
	}

	// Clears all entries from a Map.
	public void Clear()
	{
		view_as<StringMap>(this).Clear();
	}

	public bool HasValue(const int key)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		static int buffer;
		return view_as<StringMap>(this).GetValue(_key, buffer);
	}

	public bool HasArray(const int key)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		static int buffer[1];
		return view_as<StringMap>(this).GetArray(_key, buffer, 1);
	}

	public bool HasString(const int key)
	{
		static char _key[6];
		view_as<IntMapInternal>(0).__PackIntToStr(key, _key);
		static char buffer[1];
		return view_as<StringMap>(this).GetString(_key, buffer, 1);
	}

	public void Iterate(IntMapLoopCallback func, any data = 0)
	{
		if (func == INVALID_FUNCTION) {
			ThrowError("Function is invalid");
		}

		static char _key[6];
		static StringMap map;
		static IntMapInternal __internal_;
		static Action action;
		map = view_as<StringMap>(this);
		__internal_ = view_as<IntMapInternal>(0);
		action = Plugin_Continue;
		StringMapSnapshot snapshot = map.Snapshot();
		for (int x = 0; x < snapshot.Length; x++) {
			snapshot.GetKey(x, _key, 6);
			Call_StartFunction(null, func);
			Call_PushCell(this);
			Call_PushCell(x);
			Call_PushCell(__internal_.__ExtractIntFromStr(_key));
			Call_PushCell(data);
			Call_Finish(action);
			if (action == Plugin_Handled) {
				break;
			}
		}
		delete snapshot;
	}

	// Create a snapshot of the map's keys. See IntMapSnapshot.
	public IntMapSnapshot Snapshot()
	{
		return view_as<IntMapSnapshot>(view_as<StringMap>(this).Snapshot());
	}

	// Retrieves the number of elements in a map.
	property int Size
	{
		public get() {
			return view_as<StringMap>(this).Size;
		}
	}
}
