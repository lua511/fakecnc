#include "nml_oi.hh"

void
  NML_ERROR::update(CMS * cms)
{
    cms->update(error, NML_ERROR_LEN);
}

void NML_TEXT::update(CMS * cms)
{
    cms->update(text, NML_TEXT_LEN);
}

void NML_DISPLAY::update(CMS * cms)
{
    cms->update(display, NML_DISPLAY_LEN);
}

int nmlErrorFormat(NMLTYPE type, void *buffer, CMS * cms)
{
    switch (type) {
    case NML_ERROR_TYPE:
	((NML_ERROR *) buffer)->update(cms);
	break;

    case NML_TEXT_TYPE:
	((NML_TEXT *) buffer)->update(cms);
	break;

    case NML_DISPLAY_TYPE:
	((NML_DISPLAY *) buffer)->update(cms);
	break;

	// unknown type
    default:
	return 0;		// 0 signifies didn't find
    }

    return 1;			// 1 signifies found it
}

