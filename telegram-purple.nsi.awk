{
  if (match($0,"%")) {
    do {
      prev=lang;
      getline lang<"po/LINGUAS";
      l=$0;
      gsub("%",lang,l);
      if (prev!=lang) {print l}
    } while (prev!=lang);
    close("po/LINGUAS")
  } else {
    print $0
  }
}
