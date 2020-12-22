.global get_el

get_el:
	mrs x0, CurrentEL
	ands x0, x0, 0xF
	lsr x0, x0, 2

	ret
