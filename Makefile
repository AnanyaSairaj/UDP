SUBDIRS = CLIENT SERVER

subdirs:
	for dir in $(SUBDIRS); do \
	$(MAKE) -C $$dir; \
	done 
	

