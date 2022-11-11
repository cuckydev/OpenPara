/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Int.cpp -
	Int archive loader
*/

#include "Boot/Int.h"

#include "Boot/Mem.h"

namespace OpenPara
{
	namespace Int
	{
		// Int globals
		enum BlockType : uint32_t
		{
			Tim = 1,
			Vab = 2,
			Mem = 3,
			EoF = 0xFFFFFFFF,

			Read = 0,
			Complete = 0x80000000,
		};

		struct BlockFile
		{
			uint32_t size;
			char name[16];
		};
		static_assert(sizeof(BlockFile) == 0x14);

		struct BlockHeader
		{
			uint32_t type;
			uint32_t files;
			uint32_t sectors;
			uint32_t unk;
			BlockFile file[(8192 - 0x10) / 0x14];
			char pad[(8192 - 0x10) - (((8192 - 0x10) / 0x14) * 0x14)];
		};
		static_assert(sizeof(BlockHeader) == 8192);

		alignas(16) static volatile BlockHeader read_header;
		static volatile struct
		{
			void *mem;
			char *ptr;
			size_t sectors_left;
		} read_state;
		
		static Int *read_int;

		// Int functions
		Int::~Int()
		{
			// Free memory
			Mem::Free(mem);
		}

		void Int::Read(const ::CdlFILE *file)
		{
			// Enter read loop
			int read_pos = ::CdPosToInt((::CdlLOC*)&file->pos);

			while (1)
			{
				// Prepare read
				read_header.type = BlockType::Read;

				read_state.mem = (void*)&read_header;
				read_state.ptr = (char*)&read_header;
				read_state.sectors_left = 4;

				read_int = this;

				::CdReadyCallback(ReadyCallback);

				// Read block
				u_char param[4] = {};
				param[0] = CdlModeSpeed;
				::CdControlB(CdlSetmode, param, nullptr);

				::CdlLOC read_loc;
				::CdIntToPos(read_pos, &read_loc);
				::CdControl(CdlReadS, (u_char*)&read_loc, nullptr);

				// Process block
				while ((read_header.type & BlockType::Complete) == 0);
				read_pos += 4 + read_header.sectors;

				switch (read_header.type)
				{
					case BlockType::Tim | BlockType::Complete:
					{
						// Load images
						::DrawSync(0);

						char *ptr = (char*)read_state.mem;
						for (uint32_t i = 0; i < read_header.files; i++)
						{
							// Read TIM information
							::TIM_IMAGE tparam;
							::OpenTIM((u_long*)ptr);
							::ReadTIM(&tparam);

							// Load images
							::LoadImage(tparam.prect, (u_long*)tparam.paddr);
							if (tparam.mode & 0x8)
								::LoadImage(tparam.crect, (u_long*)tparam.caddr);
							
							ptr = (char*)(((uintptr_t)ptr + read_header.file[i].size + 3) & ~3);
						}

						Mem::Free(read_state.mem);

						::DrawSync(0);
						break;
					}
					case BlockType::Vab | BlockType::Complete:
					{
						Mem::Free(read_state.mem);
						break;
					}
					case BlockType::Mem | BlockType::Complete:
					{
						// Convert sizes to addresses
						char *ptr = (char*)read_state.mem;
						for (uint32_t i = 0; i < read_header.files; i++)
						{
							uint32_t size = (uint32_t)mem_file[i].ptr;
							mem_file[i].ptr = ptr;
							ptr = (char*)(((uintptr_t)ptr + size + 3) & ~3);
						}
					}
					case BlockType::EoF:
					default:
						return;
				}
			}
		}

		void Int::ReadyCallback(u_char status, u_char *result)
		{
			(void)result;

			// Check read status
			if (status != CdlDataReady)
				return;
			
			// Read data
			if (read_state.mem == (void*)&read_header)
			{
				if (read_state.sectors_left == 4)
				{
					// Read block header
					::CdGetSector((void*)&read_header, 16 / 4);

					switch (read_header.type)
					{
						case BlockType::Tim:
						case BlockType::Vab:
							// Read rest of block header
							::CdGetSector((void*)read_header.file, (2048 - 16) / 4);
							read_state.ptr += 2048;
							break;
						case BlockType::Mem:
							// Read rest of memory files
							read_state.ptr = (char*)read_int->mem_file;
							::CdGetSector(read_state.ptr, (2048 - 16) / 4);
							read_state.ptr += 2048 - 16;
							break;
						case BlockType::EoF:
							// Stop reading
							::CdControl(CdlPause, nullptr, nullptr);
							read_header.type |= BlockType::Complete;
							return;
						default:
							DebugAbort("BAD BLOCK TYPE %d\n", read_header.type);
							return;
					}

					--read_state.sectors_left;
					return;
				}
				else
				{
					// Read to memory
					::CdGetSector(read_state.ptr, 2048 / 4);
					
					// Check if we've read all sectors
					if (--read_state.sectors_left != 0)
					{
						read_state.ptr += 2048;
						return;
					}
				}

				// Init read state
				uint32_t sectors = read_header.sectors;
				read_state.sectors_left = sectors;
				read_state.mem = Mem::Alloc(sectors * 2048);
				read_state.ptr = (char*)read_state.mem;
			}
			else
			{
				// Read to memory
				::CdGetSector(read_state.ptr, 2048 / 4);
				
				// Check if we've read all sectors
				if (--read_state.sectors_left != 0)
				{
					read_state.ptr += 2048;
					return;
				}

				// Stop reading
				::CdControl(CdlPause, nullptr, nullptr);
				read_header.type |= BlockType::Complete;
			}
		}
	}
}
