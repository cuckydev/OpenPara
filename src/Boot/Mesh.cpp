/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Mesh.cpp -
	Mesh drawing
*/

#include "Boot/Mesh.h"

#include "Boot/GPU.h"

namespace OpenPara
{
	namespace Mesh
	{
		// TMD types
		struct TMDPrim
		{
			u_char olen, ilen, flag, mode;

			u_char Option() { return mode & 0x1F; }
			u_char Code()   { return (mode >> 5) & 3; }

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
			u_char u0, v0; u_short clut;
			u_char u1, v1; u_short tpage;
			u_char u2, v2; u_short pad0;
			u_char r0, g0, b0; u_char pad1;
			u_short i0, i1;
			u_short i2, pad2;
		};

		struct TMDObject
		{
			uintptr_t vert_top;
			u_long n_vert;
			uintptr_t normal_top;
			u_long n_normal;
			uintptr_t primitive_top;
			u_long n_primitive;
			long scale;
		};

		struct TMD
		{
			u_long id;
			u_long flags;
			u_long nobj;
			TMDObject obj[0];
		};

		// MIMe types
		struct VDFKey
		{
			u_long obj;
			u_long vert_top;
			u_long n_vert;
			::SVECTOR vec[0];
		};

		struct VDF
		{
			u_long keys;
			VDFKey key[0];
		};

		struct DATKey
		{
			u_short frames;
			short influence[0];
		};

		struct DAT
		{
			u_short keys;
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
			const uint32_t frames = dat_p->key[0].frames;

			// Copy vertices
			{
				const ::SVECTOR *copy_vert_p = vert_top;
				::SVECTOR *vert_p = (::SVECTOR*)0x1F800000;

				for (uint32_t i = 0; i < vertices; i++)
					*vert_p++ = *copy_vert_p++;
			}

			// Animate vertices
			{
				uint32_t frame = time >> 16;
				if (frame >= frames)
					frame = frames - 1;
				// uint32_t frame_inf1 = time & 0xFFFF;
				// uint32_t frame_inf0 = 0x10000 - frame_inf1;

				const DATKey *dat_key = &dat_p->key[0];
				const uintptr_t dat_inc = 2 + (frames << 1);

				const VDFKey *vdf_key = &vdf_p->key[0];
				const uint32_t vdf_verts = vdf_key->n_vert;
				const uintptr_t vdf_inc = 12 + (vdf_verts << 3);

				for (uint32_t key = 0; key < keys; key++)
				{
					// Read key
					const int32_t infl = dat_key->influence[frame];
					if (infl != 0)
					{
						::SVECTOR *vert_p = (::SVECTOR*)0x1F800000;
						const ::SVECTOR *key_vert_p = &vdf_key->vec[0];

						for (uint32_t v = 0; v < vdf_verts; v++, vert_p++, key_vert_p++)
						{
							int32_t x = vert_p->vx;
							int32_t y = vert_p->vy;
							int32_t z = vert_p->vz;

							int32_t kx = key_vert_p->vx;
							int32_t ky = key_vert_p->vy;
							int32_t kz = key_vert_p->vz;

							x += (kx * infl) >> 12;
							y += (ky * infl) >> 12;
							z += (kz * infl) >> 12;

							vert_p->vx = x;
							vert_p->vy = y;
							vert_p->vz = z;
						}
					}

					// Increment key
					dat_key = (const DATKey*)((uintptr_t)dat_key + dat_inc);
					vdf_key = (const VDFKey*)((uintptr_t)vdf_key + vdf_inc);
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
						::POLY_FT3 *poly = (::POLY_FT3*)GPU::g_bufferp->prip;
						GPU::g_bufferp->prip += sizeof(::POLY_FT3) >> 2;

						*((uint32_t*)&poly->r0) = *((uint32_t*)&prim_poly->r0);
						setPolyFT3(poly);

						*((uint32_t*)&poly->u0) = *((uint32_t*)&prim_poly->u0);
						*((uint32_t*)&poly->u1) = *((uint32_t*)&prim_poly->u1);
						*((uint32_t*)&poly->u2) = *((uint32_t*)&prim_poly->u2);

						// Read transform results
						uint32_t v0, v1, v2;
						gte_stsxy0(&v0);
						gte_stsxy1(&v1);
						gte_stsxy2(&v2);

						*((uint32_t*)&poly->x0) = v0;
						*((uint32_t*)&poly->x1) = v1;
						*((uint32_t*)&poly->x2) = v2;

						// Link polygon
						addPrim(&GPU::g_bufferp->ot[1], poly);
					}

					prim_p = (const TMDPrim*)((uintptr_t)prim_p + ((prim_p->ilen << 2) + 4));
				}
			}
		}
	}
}
