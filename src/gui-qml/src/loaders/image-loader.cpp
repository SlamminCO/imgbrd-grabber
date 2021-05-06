#include "image-loader.h"
#include <QUuid>
#include <utility>
#include "downloader/image-downloader.h"
#include "models/filename.h"
#include "models/image.h"
#include "models/profile.h"


ImageLoader::ImageLoader(QObject *parent)
	: Loader(parent), m_size(Size::Full), m_automatic(true), m_progress(0)
{}


QSharedPointer<Image> ImageLoader::image() const
{
	return m_image;
}
void ImageLoader::setImage(QSharedPointer<Image> image)
{
	m_image = std::move(image);
	emit imageChanged();

	if (!m_image.isNull() && m_automatic) {
		load();
	}
}

ImageLoader::Size ImageLoader::size() const
{
	return m_size;
}
void ImageLoader::setSize(ImageLoader::Size size)
{
	m_size = size;
	emit sizeChanged();
}

bool ImageLoader::automatic() const
{
	return m_automatic;
}
void ImageLoader::setAutomatic(bool automatic)
{
	m_automatic = automatic;
	emit automaticChanged();
}

qreal ImageLoader::progress() const
{
	return m_progress;
}

QString ImageLoader::source() const
{
	return m_source;
}
void ImageLoader::setSource(QString source)
{
	m_source = source;
	emit sourceChanged();
}


void ImageLoader::load()
{
	setStatus(Status::Loading);
	setError("");

	static const QMap<Size, QString> sizeToString = {
		{ Size::Full, QStringLiteral("full") },
		{ Size::Sample, QStringLiteral("sample") },
		{ Size::Thumbnail, QStringLiteral("thumb") }
	};

	Profile *profile = m_image->getProfile();
	const Filename fn = Filename(sizeToString[m_size] + "_" + QUuid::createUuid().toString().mid(1, 36) + ".%ext%");
	const QStringList paths = fn.path(*m_image.data(), profile, profile->tempPath(), 1, Filename::ExpandConditionals | Filename::Path);

	auto downloader = new ImageDownloader(profile, m_image, paths, 1, false, false, this, false, false, imageSize(), false, false);
	connect(downloader, &ImageDownloader::downloadProgress, this, &ImageLoader::downloadProgress);
	connect(downloader, &ImageDownloader::saved, this, &ImageLoader::saved);
	connect(downloader, &ImageDownloader::saved, downloader, &ImageDownloader::deleteLater);
	downloader->save();
}

void ImageLoader::downloadProgress(const QSharedPointer<Image> &img, qint64 v1, qint64 v2)
{
	Q_UNUSED(img);

	m_progress = ((qreal) v1) / ((qreal) v2);
	emit progressChanged();
}

void ImageLoader::saved(const QSharedPointer<Image> &img, const QList<ImageSaveResult> &result)
{
	ImageSaveResult res = result.first();

	if (res.result == 500) {
		setError(tr("File is too big to be displayed."));
		setSource("");
	} else if (res.result == Image::SaveResult::NotFound) {
		setError(tr("Image not found."));
		setSource("");
	} else if (res.result == Image::SaveResult::NetworkError) {
		setError(tr("Error loading the image."));
		setSource("");
	} else if (res.result == Image::SaveResult::Error) {
		setError(tr("Error saving the image."));
		setSource("");
	} else {
		img->setTemporaryPath(res.path, res.size);
		setSource("file:///" + res.path);
		setStatus(Status::Ready);
	}
}

Image::Size ImageLoader::imageSize() const
{
	if (m_size == Size::Full) {
		return Image::Size::Full;
	}
	if (m_size == Size::Sample) {
		return Image::Size::Sample;
	}
	return Image::Size::Thumbnail;
}