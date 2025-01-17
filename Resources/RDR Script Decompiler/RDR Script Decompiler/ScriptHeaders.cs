﻿using System;
using System.IO;

namespace Decompiler
{
	public class ScriptHeader
	{
		//Header Start
		public Int32 Magic { get; set; }
		public Int32 SubHeader { get; set; } //wtf?
		public Int32 CodeBlocksOffset { get; set; }
		public Int32 CodeLength { get; set; } //Total length of code
		public Int32 null04 { get; set; } //Not sure if this is the globals version
		//public Int32 ParameterCount { get; set; } //Count of paremeters to the script
		public Int32 StaticsCount { get; set; }
		public Int32 StaticsOffset { get; set; }
		public Int32 GlobalsVersion { get; set; }
		//public Int32 GlobalsCount { get; set; }
		public Int32 NativesCount { get; set; } //Native count * 4 = total block length
		public Int32 NativesOffset { get; set; }
		//public Int32 Null1 { get; set; } //unknown
		//public Int32 Null2 { get; set; } //Unknown
		//public Int32 NameHash { get; set; } //Hash of the script name at ScriptNameOffset
		//public Int32 Null3 { get; set; }
		//public Int32 ScriptNameOffset { get; set; }
		//public Int32 StringsOffset { get; set; } //Offset of the string table
		//public Int32 StringsSize { get; set; } //Total length of the string block
		//public Int32 Null4 { get; set; }
		//Header End

		//Other Vars
		public Int32 RSC7Offset;
		//public Int32[] StringTableOffsets { get; set; }
		public Int32[] CodeTableOffsets { get; set; }
		public Int32 StringBlocks { get; set; }
		public Int32 CodeBlocks { get; set; }
		//public string ScriptName { get; set; }
		public bool isRSC7 { get; private set; }

		static ScriptHeader GenerateConsoleHeader(Stream scriptStream)
		{
			ScriptHeader header = new ScriptHeader();
			IO.Reader reader = new IO.Reader(scriptStream, true);

			long header_offset = FindHeader(reader);

			Console.WriteLine("done w script header");

			scriptStream.Seek(0, SeekOrigin.Begin);

			Console.WriteLine("done w script stream");

			header.RSC7Offset = (int)header_offset;//(reader.SReadUInt32() == 0xA8D74300) ? 0x10 : 0x0;
			Console.WriteLine("RSC7 offset: {0:X}", header.RSC7Offset); // 36000


			scriptStream.Seek(header.RSC7Offset, SeekOrigin.Begin);
			header.Magic = reader.SReadInt32(); //0x0

			Console.WriteLine("Magic: {0:X}", header.Magic); // 0xA8D74300

			header.SubHeader = reader.SReadPointer(); //0x4
			header.CodeBlocksOffset = reader.SReadPointer(); //0x8
			header.CodeLength = reader.SReadInt32(); //0x10
			header.null04 = reader.SReadInt32(); //0x C
			header.StaticsCount = reader.SReadInt32(); //0x18
			header.StaticsOffset = reader.SReadPointer(); //0x24
			header.GlobalsVersion = reader.SReadInt32(); //0x28
			//header.ParameterCount = reader.SReadInt32(); //0x14
			//header.GlobalsCount = reader.SReadInt32(); //0x1C
			header.NativesCount = reader.SReadInt32(); //0x20
			header.NativesOffset = reader.SReadPointer(); //0x2C
			Console.WriteLine("NativesCount: {0:X}", header.NativesCount); // 21D
			Console.WriteLine("NativesOffset: {0:X}", header.NativesOffset); // 36030
			//header.Null1 = reader.SReadInt32(); //0x30
			//header.Null2 = reader.SReadInt32(); //0x34
			//header.NameHash = reader.SReadInt32();
			//header.Null3 = reader.SReadInt32(); //0x38
			//header.ScriptNameOffset = reader.SReadPointer(); //0x40
			//header.StringsOffset = reader.SReadPointer(); //0x44
			//header.StringsSize = reader.SReadInt32(); //0x48
			//header.Null4 = reader.ReadInt32(); //0x4C

			Console.WriteLine("CodeBlocksOffset {0:X}", header.CodeBlocksOffset); // 35DA0
			Console.WriteLine("CodeLength {0:X}", header.CodeLength); // 35D45

			//header.StringBlocks = (header.StringsSize + (1 << 14) - 1) >> 14; //(header.StringsSize + 0x3FFF) >> 14;
			header.CodeBlocks = (header.CodeLength + (1 << 14) - 1) >> 14;
			Console.WriteLine("CodeBlocks {0}", header.CodeBlocks.ToString()); // 14

			//header.StringTableOffsets = new Int32[header.StringBlocks];
			//scriptStream.Seek(header.StringsOffset + header.RSC7Offset, SeekOrigin.Begin);
			//for (int i = 0; i < header.StringBlocks; i++)
			//	header.StringTableOffsets[i] = reader.SReadPointer() + header.RSC7Offset;


			header.CodeTableOffsets = new Int32[header.CodeBlocks];
			scriptStream.Seek(header.CodeBlocksOffset , SeekOrigin.Begin);
			for (int i = 0; i < header.CodeBlocks; i++)
			{
				header.CodeTableOffsets[i] = reader.SReadPointer();
				Console.WriteLine("CodeTableOffsets[{0}]: {1:X}", i, header.CodeTableOffsets[i]); // 
			}

			/*
			scriptStream.Position = header.ScriptNameOffset;
			int data = scriptStream.ReadByte();
			header.ScriptName = "";
			while (data != 0 && data != -1)
			{
				header.ScriptName += (char)data;
				data = scriptStream.ReadByte();
			}*/

			Console.WriteLine("done reading header");

			return header;
		}

		public static ScriptHeader Generate(Stream scriptStream, bool consoleVersion)
		{
			return GenerateConsoleHeader(scriptStream);
		}

		public static long FindHeader(IO.Reader reader)
		{
			while (reader.BaseStream.Position < reader.BaseStream.Length)
			{
				if (reader.SReadInt32() == -1462287616) // 0xA8D74300
				{
					Console.WriteLine("found script header");
					reader.BaseStream.Position -= 4;

					Console.WriteLine("pos: {0}", reader.BaseStream.Position.ToString());
					return reader.BaseStream.Position;
				}
				else
				{
					Console.WriteLine("reading custom script header"); // get call here with both values
					reader.BaseStream.Position += 4092;
				}
			}
			return 0;
		}

		private ScriptHeader()
		{ }
	}

}
