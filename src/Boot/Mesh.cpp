/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Mesh.cpp -
	Mesh drawing
*/

#include "Boot/Mesh.h"

#include "Boot/GPU.h"

// C externs
extern "C" void OpenPara_Mesh_MIMe_asm(::SVECTOR *out, const ::SVECTOR *in, uint32_t n_vert, int32_t weight);

namespace OpenPara
{
	namespace Mesh
	{
		// TMD types
		struct TMDPrim
		{
			uint8_t olen, ilen, flag, mode;

			uint8_t Option() { return mode & 0x1F; }
			uint8_t Code()   { return (mode >> 5) & 3; }

			bool IsUnlit()       { return ((flag >> 0) & 1) == 0; }
			bool IsDoubleSided() { return ((flag >> 1) & 1) == 1; }
			bool IsGradient()    { return ((flag >> 2) & 1) == 1 || mode == 0x35 || mode == 0x31 || mode == 0x39 || mode == 0x3D || mode == 0x3B || mode == 0x3F; }

			bool IsBright()       { return ((Option() >> 0) & 1) == 0; }
			bool IsTransluscent() { return ((Option() >> 1) & 1) == 0; }
			bool IsTextured()     { return ((Option() >> 2) & 1) == 0; }
			bool IsQuad()         { return ((Option() >> 3) & 1) == 0; }
			bool IsShaded()       { return ((Option() >> 4) & 1) == 0; }
		};

		struct TMDPrim_PolyFT3
		{
			TMDPrim prim;
			uint8_t u0, v0; uint16_t clut;
			uint8_t u1, v1; uint16_t tpage;
			uint8_t u2, v2; uint16_t pad0;
			uint8_t r0, g0, b0; uint8_t pad1;
			uint16_t i0, i1;
			uint16_t i2, pad2;
		};

		struct TMDObject
		{
			uintptr_t vert_top;
			uint32_t n_vert;
			uintptr_t normal_top;
			uint32_t n_normal;
			uintptr_t primitive_top;
			uint32_t n_primitive;
			int32_t scale;
		};

		struct TMD
		{
			uint32_t id;
			uint32_t flags;
			uint32_t nobj;
			TMDObject obj[0];
		};

		// MIMe types
		struct VDFKey
		{
			uint32_t obj;
			uint32_t vert_top;
			uint32_t n_vert;
			::SVECTOR vec[0];
		};

		struct VDF
		{
			uint32_t keys;
			VDFKey key[0];
		};

		struct DATKey
		{
			uint16_t frames;
			int16_t influence[0];
		};

		struct DAT
		{
			uint16_t keys;
			DATKey key[0];
		};

		// Mesh functions
		void DrawMIMe(const void *tmd, uint32_t time, const void *vdf, const void *dat)
		{
			// Read TMD
			const TMD *tmd_p = (const TMD*)tmd;

			const ::SVECTOR *vert_top = (const ::SVECTOR*)(tmd_p->obj[0].vert_top + (uintptr_t)&tmd_p->obj[0]);
			const uint32_t vertices = tmd_p->obj[0].n_vert;

			// Read VDF
			const VDF *vdf_p = (const VDF*)vdf;

			// Read DAT
			const DAT *dat_p = (const DAT*)dat;

			const uint32_t keys = dat_p->keys;
			uint32_t frames = dat_p->key[0].frames;
			if (frames != 0)
				frames--;

			// Copy vertices
			{
				const ::SVECTOR *copy_vert_p = vert_top;
				::SVECTOR *vert_p = (::SVECTOR*)0x1F800000;

				for (uint32_t i = 0; i < vertices; i++)
					*vert_p++ = *copy_vert_p++;
			}

			// Animate vertices
			if (vdf != nullptr)
			{
				// Read VDF and DAT keys
				const DATKey *dat_key = &dat_p->key[0];
				const uintptr_t dat_inc = 2 + (frames << 1);

				const VDFKey *vdf_key = &vdf_p->key[0];
				const uint32_t vdf_verts = vdf_key->n_vert;
				const uintptr_t vdf_inc = 12 + (vdf_verts << 3);
				
				const uint32_t frame = time >> 8;
				const uint32_t frame_sub = time & 0xFF;

				if (frame >= frames)
				{
					// Uninterpolated animation
					for (uint32_t key = 0; key < keys; key++)
					{
						// Read key
						const int32_t infl = dat_key->influence[frames];

						if (infl != 0)
						{
							::SVECTOR *vert_p = (::SVECTOR*)0x1F800000;
							const ::SVECTOR *key_vert_p = &vdf_key->vec[0];
							OpenPara_Mesh_MIMe_asm(vert_p, key_vert_p, vdf_key->n_vert, infl);
						}

						// Increment key
						dat_key = (const DATKey*)((uintptr_t)dat_key + dat_inc);
						vdf_key = (const VDFKey*)((uintptr_t)vdf_key + vdf_inc);
					}
				}
				else
				{
					// Interpolated animation
					for (uint32_t key = 0; key < keys; key++)
					{
						// Read key
						int32_t infl;
						if (frame_sub != 0)
						{
							const int32_t infl0 = dat_key->influence[frame];
							const int32_t infl1 = dat_key->influence[frame + 1];
							infl = infl0 + (((int32_t)(infl1 - infl0) * (int32_t)frame_sub) >> 8);
						}
						else
						{
							infl = dat_key->influence[frame];
						}

						if (infl != 0)
						{
							::SVECTOR *vert_p = (::SVECTOR*)0x1F800000;
							const ::SVECTOR *key_vert_p = &vdf_key->vec[0];
							OpenPara_Mesh_MIMe_asm(vert_p, key_vert_p, vdf_key->n_vert, infl);
						}

						// Increment key
						dat_key = (const DATKey*)((uintptr_t)dat_key + dat_inc);
						vdf_key = (const VDFKey*)((uintptr_t)vdf_key + vdf_inc);
					}
				}
			}

			// Display primitives
			{
				uint32_t prims = tmd_p->obj[0].n_primitive;
				const TMDPrim *prim_p = (const TMDPrim*)(tmd_p->obj[0].primitive_top + (uintptr_t)&tmd_p->obj[0]);

				const ::SVECTOR *vert_p = (::SVECTOR*)0x1F800000;

				for (uint32_t i = 0; i < prims; i++)
				{
					// Check prim
					if (*((uint32_t*)prim_p) == 0x25010607)
					{
						// Get primitive
						const TMDPrim_PolyFT3 *prim_poly = (const TMDPrim_PolyFT3*)prim_p;

						// Transform vertices
						gte_ldv3(&vert_p[prim_poly->i0], &vert_p[prim_poly->i1], &vert_p[prim_poly->i2]);
						gte_rtpt();

						// Copy polygon
						::POLY_FT3 *poly = GPU::GetPrim<::POLY_FT3>();

						*((uint32_t*)&poly->r0) = *((uint32_t*)&prim_poly->r0);
						setPolyFT3(poly);

						*((uint32_t*)&poly->u0) = *((uint32_t*)&prim_poly->u0);
						*((uint32_t*)&poly->u1) = *((uint32_t*)&prim_poly->u1);
						*((uint32_t*)&poly->u2) = *((uint32_t*)&prim_poly->u2);

						// Perform normal check
						int32_t nsign;
						gte_nclip_b();
						gte_stopz(&nsign);

						if (nsign > 0)
						{
							// Average Z values
							gte_avsz3_b();

							// Read transform results
							uint32_t v0, v1, v2;
							gte_stsxy0(&v0);
							gte_stsxy1(&v1);
							gte_stsxy2(&v2);

							*((uint32_t*)&poly->x0) = v0;
							*((uint32_t*)&poly->x1) = v1;
							*((uint32_t*)&poly->x2) = v2;

							// Read Z index
							uint32_t otz;
							gte_stotz(&otz);
							otz >>= 3;

							// Link polygon
							if (otz != 0 && otz < GPU::OT::Length)
							{
								addPrim(&GPU::g_bufferp->ot[otz], poly);
								GPU::InsertPrim<::POLY_FT3>();
							}
						}
					}

					prim_p = (const TMDPrim*)((uintptr_t)prim_p + ((prim_p->ilen << 2) + 4));
				}
			}
		}
	}
}
