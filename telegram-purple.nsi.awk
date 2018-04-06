{
  if (match($0,"變")) {
    do {
      prev=lang;
      getline lang<"po/LINGUAS";
      l=$0;
      gsub("變",lang,l);
      if (prev!=lang) {print l}
    } while (prev!=lang);
    close("po/LINGUAS")
  } else {
    print $0
  }
}
