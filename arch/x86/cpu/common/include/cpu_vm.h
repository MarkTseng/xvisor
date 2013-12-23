#ifndef __CPU_VM_H__
#define __CPU_VM_H__

#include <multiboot.h>
#include <vm/amd_vmcb.h>
#include <processor_flags.h>
#include <cpu_features.h>
#include <vmm_types.h>

#define USER_CMD_ENABLE		0
#define USER_CMD_DISABLE	1
#define USER_CMD_TEST		9

#define USER_ITC_SWINT		(1 << 0)
#define USER_ITC_TASKSWITCH	(1 << 1)
#define USER_ITC_SYSCALL	(1 << 2)
#define USER_ITC_IRET		(1 << 3)
#define USER_SINGLE_STEPPING	(1 << 4)
#define USER_UNPACK		(1 << 5)
#define USER_ITC_ALL		(0xFF)

#define USER_TEST_SWITCHMODE	1

#define	GUEST_PADDR_MBI	0x2d0e0UL

#define IO_INTCPT_TBL_SZ	(12 << 10)
#define MSR_INTCPT_TBL_SZ	(8 << 10)

struct vcpu_hw_context {
	struct vmcb *vmcb;
	struct vmcs *vmcs;

	unsigned int asid;
	unsigned long n_cr3;  /* [Note] When #VMEXIT occurs with
			       * nested paging enabled, hCR3 is not
			       * saved back into the VMCB (vol2 p. 409)???*/

	physical_addr_t io_intercept_table;
	physical_addr_t msr_intercept_table;

	/* Intel VMX only */
	unsigned int		msr_count;
	struct vmx_msr_entry	*msr_area;

	unsigned int		host_msr_count;
	struct vmx_msr_entry	*host_msr_area;

	int itc_flag;  /* flags specifying which interceptions were
			  registered for this vm. */
	int itc_skip_flag;

	/* on & exit handler */
	void (*vcpu_run) (struct vcpu_hw_context *context);
	void (*vcpu_exit) (struct vcpu_hw_context *context);

	/* event handler pointer */
	void (*vcpu_handle_wrmsr) (struct vcpu_hw_context *context);
	void (*vcpu_handle_exception) (struct vcpu_hw_context *context);
	void (*vcpu_handle_swint) (struct vcpu_hw_context *context);
	void (*vcpu_handle_npf) (struct vcpu_hw_context *context);
	void (*vcpu_handle_vmcall) (struct vcpu_hw_context *context);
	void (*vcpu_handle_iret) (struct vcpu_hw_context *context);
	void (*vcpu_handle_cr3_write) (struct vcpu_hw_context *context);
	void (*vcpu_handle_popf) (struct vcpu_hw_context *context);
};

#define VMM_CS32	8   /* entry 1 of gdt ?? Xvisor also uses it. FIXME: */
#define VMM_DS32	16  /* entry 2 of gdt ?? Xvisor also uses it. FIXME: */
#define VMM_CS64	40  /* entry 7 of gdt */
#define VMM_DS64	56  /* entry 5 of gdt */

extern void print_page_errorcode(u64 errcode);

extern physical_addr_t cpu_create_vcpu_intercept_table(size_t size);
extern void cpu_disable_vcpu_intercept(struct vcpu_hw_context *context, int flags);
extern void cpu_enable_vcpu_intercept(struct vcpu_hw_context *context, int flags);
extern int cpu_init_vcpu_hw_context(struct cpuinfo_x86 *cpuinfo, struct vcpu_hw_context *context,
				    unsigned long vmm_pmem_start,
				    unsigned long vmm_pmem_size);
extern void cpu_boot_vcpu(struct vcpu_hw_context *context);

extern int cpu_enable_vm_extensions(struct cpuinfo_x86 *cpuinfo);

#endif /* __CPU_VM_H__ */
