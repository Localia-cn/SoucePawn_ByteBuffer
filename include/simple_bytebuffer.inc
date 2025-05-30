
#if defined _simple_bytebuffer_included
 #endinput
#endif
#define _simple_bytebuffer_included



#define MAX_BYTE_BUFFER_SIZE 4096 //You can adjust the limit size according to your needs.


/**
 * @note  A bytebuffer contains a large char array. Make it global
 *        and reuse it with Reset() instead of creating
 *        new ones repeatedly."
 * 
 * @param bytes         The buffer used to store byte data.
 * @param size          The size of data already written to the buffer.
 * @param readPos       The current read offset of this buffer (starting at 0).
 */
enum struct ByteBuffer
{
	char bytes[MAX_BYTE_BUFFER_SIZE+1];//+1后在末尾额外添加\0，能保证ReadString检查strlen时不会越界
	int readPos;
	int size;


	/**
	 * Reset the read position and written data size of the bytebuffer.
	 * @note  You should call this function after processing each data segment 
	 *        before handling the next one.
	 */
	void Reset()
	{
		this.size = 0;
		this.readPos = 0;
	}

	/**
	 * Copy and load data into the bytebuffer.
	 * @note  You can use this function multiple times to load data, with 
	 *        each invocation appending the data to the end of the bytebuffer.
	 *        
	 * @param source        The source data buffer to load.
	 * @param startPos      The first byte index to read from the source data buffer.
	 * @param addSize       The size of the data to add.
	 * @error               Insufficient remaining space in the bytebuffer or 
	 *                      addSize < 0 or startPos < 0.
	 */
	void LoadBytes(const char[] source,int startPos,int addSize)
	{
		if(startPos < 0)
		{
			ThrowError("startPos(%i) < 0", startPos);
		}

		if(addSize < 0)
		{
			ThrowError("addSize(%i) < 0", addSize);
		}

		if(this.size + addSize > MAX_BYTE_BUFFER_SIZE)
		{
			ThrowError("Buffer overflow detected. Cannot write %i more bytes to the buffer, remain size %i", addSize, MAX_BYTE_BUFFER_SIZE);
		}

		/*for(int i = 0 ;i < addSize;i++)
		{
			this.bytes[this.size + i] = source[startPos+i];
		}*/
		
		// 优化方案：每次处理4字节
		int blocks = addSize / 4;
		int pos;
		for (int i=0; i<blocks; i++) 
		{
			pos = i*4;
			this.bytes[this.size + pos] = source[startPos + pos];
			pos ++; 
			this.bytes[this.size + pos] = source[startPos + pos];
			pos ++; 
			this.bytes[this.size + pos] = source[startPos + pos];
			pos ++; 
			this.bytes[this.size + pos] = source[startPos + pos];
		}
		
		// 处理小于cell剩余字节
		int remainder = addSize % 4;
		for (int i=0; i<remainder; i++) 
		{
			this.bytes[this.size + blocks*4 + i] = source[startPos + blocks*4 + i];
		}



		this.size += addSize;
	
	}

	/**
	 * Writes a 8bit unsigned integer to the bytebuffer.
	 * @note  The range of uint8 is 0-255. Writing a value outside this range
	 *        will not throw an error, but it will result in reading an 
	 *        incorrect numerical value.
	 *  
	 * @param val           Integer to write (value will be written as 8bit).
	 * @error               Insufficient remaining space in the bytebuffer.
	 */
	void WriteUInt8(int val)
	{
		
		if(this.size  + 1> MAX_BYTE_BUFFER_SIZE)
		{
			ThrowError("Buffer overflow detected. Cannot write one more byte to the buffer of size %i", MAX_BYTE_BUFFER_SIZE);
		}

		this.bytes[this.size] =  val & 0xFF;
		this.size += 1;

	}

	/**
	 * Writes a boolean to the bytebuffer.
	 *
	 * @param val           Boolean to write (value will be written as 8bit).
	 * @error               Insufficient remaining space in the bytebuffer.
	 */
	void WriteBool(bool val)
	{
		this.WriteUInt8(val?1:0);
	}

	/**
	 * Writes a 16bit unsigned integer to the ByteBuffer.
	 * @note  The range of uint16 is 0-65535. Writing a value outside this range
	 *        will not throw an error, but it will result in reading an 
	 *        incorrect numerical value.
	 *  
	 * @param val           Integer to write (value will be written as 16bit).
	 * @error               Insufficient remaining space in the bytebuffer.
	 */
	void WriteUInt16(int val)
	{
		
		if(this.size  + 2> MAX_BYTE_BUFFER_SIZE)
		{
			ThrowError("Buffer overflow detected. Cannot write %i more bytes to the buffer, remain size %i", 2, MAX_BYTE_BUFFER_SIZE - this.size);
		}

		this.bytes[this.size] =  val & 0xFF;
		this.bytes[this.size+1] =  (val >>8) & 0xFF;
		this.size += 2;

	}

	/**
	 * Writes a normal integer to the bytebuffer.
	 *
	 * @param val           Integer to write (value will be written as 32bit).
	 * @error               Insufficient remaining space in the bytebuffer.
	 */
	void WriteInt32(int val)
	{
		
		if(this.size  + 4> MAX_BYTE_BUFFER_SIZE)
		{
			ThrowError("Buffer overflow detected. Cannot write %i more bytes to the buffer, remain size %i", 4, MAX_BYTE_BUFFER_SIZE - this.size);
		}

		this.bytes[this.size] =  val & 0xFF;
		this.bytes[this.size+1] =  (val >>8) & 0xFF;
		this.bytes[this.size+2] =  (val >>16) & 0xFF;
		this.bytes[this.size+3] =  (val >>24) & 0xFF;
		this.size += 4;

	}

	/**
	 * Writes a floating point number to the bytebuffer.
	 *
	 * @param val           Number to write.
	 * @error               Insufficient remaining space in the bytebuffer.
	 */
	void WriteFloat(float val)
	{
		this.WriteInt32(view_as<int>(val));
	}

	/**
	 * Writes a 3D normal vector to the bytebuffer.
	 *
	 * @param vec           Vector to write.
	 * @error               Insufficient remaining space in the bytebuffer.
	 */
	void WriteVector(float vec[3])
	{
		this.WriteFloat(vec[0]);
		this.WriteFloat(vec[1]);
		this.WriteFloat(vec[2]);
	}

	/**
	 * Writes a string to the bytebuffer.
	 *
	 * @param string        Text string to write.
	 * @return              Number of characters written to the buffer, not including 
	 *                      the null terminator.
	 * @error               Insufficient remaining space in the bytebuffer.
	 */
	int WriteString(const char[] string)
	{
		
		int addSize = strlen(string) + 1;
        if(this.size + addSize > MAX_BYTE_BUFFER_SIZE)
            ThrowError("Buffer overflow detected. Cannot write %i more bytes to the buffer, remain size %i", addSize, MAX_BYTE_BUFFER_SIZE - this.size);

        // 使用strcopy优化字符串写入
		strcopy(this.bytes[this.size], addSize, string);

        this.size += addSize;

		return addSize -1;
	}

	/**
	 * Extract a 8bit unsigned integer from the bytebuffer and store as a normal integer.
	 *
	 * @return              Integer value read.
	 * @error               Not enough data to read.
	 */
	int ReadUInt8()
	{
		if(this.readPos + 1> this.size)
		{
			ThrowError("Not enough data to read. Required: %i bytes, available: %i bytes.",1, this.size - this.readPos);
		}
		this.readPos += 1;
		return this.bytes[this.readPos-1];
	}

	/**
	 * Read a 8bit unsigned integer from the bytebuffer and return its boolean representation.
	 *
	 * @return              return true if it is non-zero, otherwise return false.
	 * @error               Not enough data to read.
	 */
	bool ReadBool()
	{
		return this.ReadUInt8()?true:false;
	}

	/**
	 * Extract a 16bit unsigned integer from the bytebuffer and store as a normal integer.
	 *
	 * @return              Integer value read.
	 * @error               Not enough data to read.
	 */
	int ReadUInt16()
	{
		if(this.readPos + 2> this.size)
		{
			ThrowError("Not enough data to read. Required: %i bytes, available: %i bytes.",2, this.size - this.readPos);
		}
		this.readPos += 2;
		return (this.bytes[this.readPos-2] << 0) | (this.bytes[this.readPos-1] << 8);
	}

	/**
	 * Reads a normal integer from the bytebuffer.
	 *
	 * @return              Integer value read.
	 * @error               Not enough data to read.
	 */
	int ReadInt32()
	{
		
		if(this.readPos + 4> this.size)
		{
			ThrowError("Not enough data to read. Required: %i bytes, available: %i bytes.",4, this.size - this.readPos);
		}
		this.readPos += 4;
		return (this.bytes[this.readPos-4] << 0) | (this.bytes[this.readPos-3] << 8) | (this.bytes[this.readPos-2] << 16) | (this.bytes[this.readPos-1] << 24);
	}

	/**
	 * Reads a floating point number from the bytebuffer.
	 *
	 * @return              Floating point value read. 
	 * @error               Not enough data to read.
	 */
	float ReadFloat()
	{
		return view_as<float>(this.ReadInt32());
	}

	/**
	 * Reads a 3D normal vector from the bytebuffer.
	 *
	 * @vec                 Destination vector array.
	 * @error               Not enough data to read.
	 */
	void ReadVector(float vec[3])
	{
		vec[0] = this.ReadFloat();
		vec[1] = this.ReadFloat();
		vec[2] = this.ReadFloat();
		
	}
	
	/**
	 * Reads a string from the bytebuffer.
	 *
	 * @param dest          Destination string buffer to copy to.
	 * @param destLen       Destination buffer length (includes null terminator).
	 * @return              Number of characters written to the buffer, not including 
	 *                      the null terminator.
	 * @error               Destination buffer too small or no string found.
	 */
	int ReadString(char[] dest,int destLen)
	{
		// 确保至少有1字节空间存放\0
    	if (destLen < 1)
        	ThrowError("Destination buffer too small");
		
		int startPos = this.readPos;
		if(startPos + 1> this.size)
		{
			ThrowError("Not enough data to read string.");
		}

        int str_size = strlen(this.bytes[startPos]) +1;//计算总长度（包含\0）
		
        // 检查字符串总长度位置是否在合理范围
        if(startPos + str_size > this.size)
            ThrowError("Invalid string termination");
        
        if(str_size > destLen)
            ThrowError("Destination buffer too small.need %i ,now %i",str_size,destLen);
        
        // 使用strcopy优化字符串读取
        strcopy(dest, str_size, this.bytes[startPos]);

        this.readPos += str_size;
        return str_size - 1; // 返回实际字符数（不包含\0）
	}
	
	/**
	 * Overwrites a 2-byte uint16 at a specified position in the bytebuffer.
	 * @note  You should overwrite at the correct starting position, otherwise
	 *        it may overwrite other data and cause unforeseen issues.The range
	 *        of uint16 is 0-65535. Writing a value outside this range will not
	 *        throw an error, but it will result in reading an incorrect numerical
	 *        value. 
 	 *        
	 * @param val           Integer to overwrite.(value will be written as 16bit).
	 * @param startPosition Position of the first byte to be overwritten.
	 * @error               The overwrite range exceeds the written boundary in
	 *                      the bytebuffer.
	 */
	void OverwriteUInt16(int val,int startPosition)
	{
		if(startPosition < 0 || startPosition + 2 > this.size)
		{
			ThrowError("The overwrite range exceeds the written boundary in the bytebuffer.");
		}

		this.bytes[startPosition] =  val & 0xFF;
		this.bytes[startPosition+1] =  (val >>8) & 0xFF;

	}

	/**
	 * Overwrites a 4-byte int32 at a specified position in the bytebuffer.
	 * @note  You should overwrite at the correct starting position, otherwise
	 *        it may overwrite other data and cause unforeseen issues. 
 	 *        
	 * @param val           Integer to overwrite.(value will be written as 32bit).
	 * @param startPosition Position of the first byte to be overwritten.
	 * @error               The overwrite range exceeds the written boundary in
	 *                      the bytebuffer.
	 */
	void OverwriteInt32(int val,int startPosition)
	{
		
		if(startPosition < 0 || startPosition + 4 > this.size)
		{
			ThrowError("The overwrite range exceeds the written boundary in the bytebuffer.");
		}

		this.bytes[startPosition] =  val & 0xFF;
		this.bytes[startPosition+1] =  (val >>8) & 0xFF;
		this.bytes[startPosition+2] =  (val >>16) & 0xFF;
		this.bytes[startPosition+3] =  (val >>24) & 0xFF;

	}

}
