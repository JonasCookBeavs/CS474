#include <stdio.h>

// Simulated physical RAM (all of it)
unsigned char pmem[65536] = {0};

/*
 * Set up the page table (in page 0 of physical memory) so that a given
 * virtual page number maps to a physical page number.
 *
 * Input:
 *      vpagenum: the virtual page number
 *      ppagenum: the physical page to map the virtual page to
 */
void set_page_table_entry(int vpagenum, int ppagenum)
{
    pmem[vpagenum] = ppagenum;
}

/*
 * For a given address, returns the page number.
 *
 * Input:
 *      addr: an address (physical or virtual)
 *
 * Output:
 *      The page number of the address.
 */
int get_addr_page(int addr)
{
    return (addr >> 12);
}

/*
 * For a given address, returns the offset.
 *
 * Input:
 *      addr: an address (physical or virtual)
 *
 * Output:
 *      The offset of the address.
 */
int get_addr_offset(int addr)
{
    return (addr & 0xFFF);
}

/*
 * For a given virtual address, returns the physical address.
 *
 * Input:
 *      vaddr: the address to map.
 *
 * Output:
 *      The physical address corresponding to the virtual address.
 *
 *      Returns 0 if the virtual address is not mapped to a page.
 */
int vaddr_to_paddr(int vaddr)
{
    if(pmem[get_addr_page(vaddr)] != 0){
        return (pmem[get_addr_page(vaddr)] << 12) | get_addr_offset(vaddr);
    }
    return 0;
}

/*
 * Wrapper function to print a mapped address.
 *
 * Input:
 *      vaddr: the virtual address to print the mapping of.
 */
void print_mapped_address(int vaddr)
{
    int paddr = vaddr_to_paddr(vaddr);
    int ppage = get_addr_page(paddr);
    int offset = get_addr_offset(paddr);

    if (ppage == 0)
        printf("%04x -> PAGE FAULT\n", vaddr);
    else
        printf("%04x -> %04x (page %d, offset %d)\n", vaddr, paddr, ppage, offset);
}

/*
 * Main
 */
int main(void)
{
    set_page_table_entry(0, 4);  // virtual address 0-4095
    set_page_table_entry(1, 2);  // virtual address 4096-8191
    set_page_table_entry(2, 13); // virtual address 8192-12287
    set_page_table_entry(5, 3);  // virtual address 20480-24575

    print_mapped_address(0);
    print_mapped_address(1);
    print_mapped_address(2);
    print_mapped_address(4095);
    print_mapped_address(4096);
    print_mapped_address(12287);
    print_mapped_address(12288); // page fault
    print_mapped_address(20479); // page fault
    print_mapped_address(20480);
    print_mapped_address(20500);
    print_mapped_address(24575);
    print_mapped_address(24576); // page fault
    print_mapped_address(30000); // page fault

    /* Expected output from the above:

    0000 -> 4000 (page 4, offset 0)
    0001 -> 4001 (page 4, offset 1)
    0002 -> 4002 (page 4, offset 2)
    0fff -> 4fff (page 4, offset 4095)
    1000 -> 2000 (page 2, offset 0)
    2fff -> dfff (page 13, offset 4095)
    3000 -> PAGE FAULT
    4fff -> PAGE FAULT
    5000 -> 3000 (page 3, offset 0)
    5014 -> 3014 (page 3, offset 20)
    5fff -> 3fff (page 3, offset 4095)
    6000 -> PAGE FAULT
    7530 -> PAGE FAULT

    */
}


