(begin
	(define make-point (lambda (x y) (begin 
										(define a (list x y))
										(set-property "object-name" "point" a)
										(set-property "size" 0 a)
									  )))
)