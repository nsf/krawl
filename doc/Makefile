ASCIIDOC_FLAGS := -a theme=flask -a toc2 -a icons -a iconsdir=icons
DOCS := \
	crawl_by_example.txt \
	typesystem.txt \

DOCS_HTML := $(patsubst %.txt,%.html,$(DOCS))

all: $(DOCS_HTML)
deploy: $(addprefix $(HOME)/storage/web/,$(DOCS_HTML))

crawl.lang: ../extra/crawl.lang
	cp ../extra/crawl.lang .

$(HOME)/storage/web/%.html: %.html
	cp $*.html $(HOME)/storage/web/$*.html

%.html: %.txt crawl.lang
	asciidoc $(ASCIIDOC_FLAGS) $*.txt

clean:
	rm -rf $(DOCS_HTML)
